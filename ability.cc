#include "config.h"

#include <string.h>

#ifdef DOS
#include <conio.h>
#endif

#ifdef LINUX
#include "linuxlib.h"
#endif

#include "externs.h"

#include "ability.h"
#include "bang.h"
#include "beam.h"
#include "effects.h"
#include "items.h"
#include "it_use2.h"
#include "macro.h"
#include "misc.h"
#include "monplace.h"
#include "player.h"
#include "randart.h"
#include "religion.h"
#include "spell.h"
#include "spells1.h"
#include "spells2.h"
#include "spells3.h"
#include "skills.h"
#include "skills2.h"
#include "spells3.h"
#include "stuff.h"
#include "view.h"

/*
Activates a menu which gives player access to all of their non-spell
special abilities - Eg naga's spit poison, or the Invocations you get
from worshipping. Generated dynamically - the function checks to see which
abilities you have every time.
*/
void species_ability(void) /* Now handles all special abilities */
{

        if (you[0].berserker > 0)
        {
                mpr("You're too berserk!");
                return;
        }

        if (you[0].conf > 0)
        {
                mpr("You're too confused!");
                return;
        }

        struct dist abild [1];
        struct bolt beam [1];
        struct dist spd [1];

        int ability [120];
        int ability_fail [120];
        char abil_c = 0;
        unsigned char abil_used;
        unsigned char spc, spc2;
        char keyi;
        int i = 0;

        for (i = 0; i < 120; i ++)
        {
                ability [i] = 0;
                ability_fail [i] = 0;
        }

        switch(you[0].species)
        {
                case 13: // Naga
                        ability [abil_c] = 1;
                        ability_fail [abil_c] = 20 - you[0].xl;
                        abil_c ++;
                        break;
                case 14: // Gnome
                        ability [abil_c] = 2;
                        ability_fail [abil_c] = 20 - you[0].xl;
                        abil_c ++;
                        break;
        }

        if (you[0].species != 13 && you[0].mutation [28] > 0) // spit poison
        {
                ability [abil_c] = 1;
                ability_fail [abil_c] = 30 - you[0].mutation [28] * 10 - you[0].xl;
                abil_c ++;
        }

        if (you[0].species != 14 && (you[0].mutation [29] > 0 || scan_randarts(18) > 0)) // sense surr
        {
                ability [abil_c] = 2;
                ability_fail [abil_c] = 30 - you[0].mutation [29] * 10 - you[0].xl;
                abil_c ++;
        }

        if (you[0].attribute [5] == 5 || you[0].mutation [30] > 0 || (you[0].species == 18 && you[0].xl > 6)) // breathe fire
        {
                ability [abil_c] = 4;
                ability_fail [abil_c] = 50 - you[0].mutation [30] * 10 - you[0].xl - (you[0].attribute [5] == 5) * 20;
                if (you[0].species == 18) ability_fail [abil_c] = 30 - you[0].xl;
                abil_c ++;
        }

        if (you[0].species == 19 && you[0].xl > 6) // breathe frost
        {
                ability [abil_c] = 6;
                ability_fail [abil_c] = 30 - you[0].xl;
                abil_c ++;
        }

        if (you[0].species == 20 && you[0].xl > 6) // breathe poison
        {
                ability [abil_c] = 7;
                ability_fail [abil_c] = 30 - you[0].xl;
                abil_c ++;
        }

        if (you[0].species == 21 && you[0].xl > 6) // spit acid
        {
                ability [abil_c] = 9;
                ability_fail [abil_c] = 30 - you[0].xl;
                abil_c ++;
        }

        if (you[0].species == 23 && you[0].xl > 6) // breathe lightning
        {
                ability [abil_c] = 8;
                ability_fail [abil_c] = 30 - you[0].xl;
                abil_c ++;
        }

        if (you[0].species == 24 && you[0].xl > 6) // breathe power
        {
                ability [abil_c] = 10;
                ability_fail [abil_c] = 30 - you[0].xl;
                abil_c ++;
        }

        if (you[0].species == 25 && you[0].xl > 6) // spit sticky flame
        {
         ability [abil_c] = 12;
         ability_fail [abil_c] = 30 - you[0].xl;
         abil_c ++;
        }

        if (you[0].species == 26 && you[0].xl > 6) // breathe steam
        {
         ability [abil_c] = 13;
         ability_fail [abil_c] = 20 - you[0].xl;
         abil_c ++;
        }

        if (you[0].mutation [31] > 0 || scan_randarts(15) > 0) // blink
        {
                ability [abil_c] = 5;
                ability_fail [abil_c] = 30 - you[0].mutation [31] * 10;
                abil_c ++;
        }



        if (wearing_amulet(35) != 0 || scan_randarts(17) > 0)
        {
                ability [abil_c] = 11;
                ability_fail [abil_c] = 50 - you[0].xl * 2;
                if (you[0].species == 7 || you[0].species == 8 || you[0].species == 10 || you[0].species == 15) ability_fail [abil_c] -= 10;
                if (you[0].species == 16) ability_fail [abil_c] -= 30; /* troll */
                abil_c ++;
        }

        if (you[0].species == 36 && you[0].xl >= 5 && you[0].duration [12] == 0 && you[0].lev == 0) // kenku can fly, but only from the ground (until lev 15, when it becomes permanent until revoked)
        {
                ability [abil_c] = 14;
                ability_fail [abil_c] = 45 - you[0].xl * 3;
                abil_c ++;
        } /* Kenku can fly */

/* Assorted demonic powers follow: */

        if (you[0].mutation [45] != 0) // Summon minor demon
        {
         ability [abil_c] = 15;
         ability_fail [abil_c] = 27 - you[0].xl;
         abil_c ++;
        }

        if (you[0].mutation [46] != 0) // Summon demon
        {
         ability [abil_c] = 16;
         ability_fail [abil_c] = 40 - you[0].xl;
         abil_c ++;
        }

        if (you[0].mutation [47] != 0) // Hellfire
        {
         ability [abil_c] = 17;
         ability_fail [abil_c] = 50 - you[0].xl;
         abil_c ++;
        }

        if (you[0].mutation [48] != 0) // Torment
        {
         ability [abil_c] = 18;
         ability_fail [abil_c] = 60 - you[0].xl;
         abil_c ++;
        }

        if (you[0].mutation [49] != 0) // Raise Dead
        {
         ability [abil_c] = 19;
         ability_fail [abil_c] = 50 - you[0].xl;
         abil_c ++;
        }

        if (you[0].mutation [50] != 0) // Control Demon
        {
         ability [abil_c] = 20;
         ability_fail [abil_c] = 35 - you[0].xl;
         abil_c ++;
        }

        if (you[0].mutation [51] != 0) // Gate to Pan
        {
         ability [abil_c] = 21;
         ability_fail [abil_c] = 57 - you[0].xl * 2;
         abil_c ++;
        }

        if (you[0].mutation [53] != 0) // Channel from Hell
        {
         ability [abil_c] = 22;
         ability_fail [abil_c] = 30 - you[0].xl;
         abil_c ++;
        }

        if (you[0].mutation [55] != 0) // Throw flame
        {
         ability [abil_c] = 23;
         ability_fail [abil_c] = 10 - you[0].xl;
         abil_c ++;
        }

        if (you[0].mutation [56] != 0) // Throw frost
        {
         ability [abil_c] = 24;
         ability_fail [abil_c] = 10 - you[0].xl;
         abil_c ++;
        }

        if (you[0].mutation [57] != 0) // Bolt of Draining
        {
         ability [abil_c] = 25;
         ability_fail [abil_c] = 30 - you[0].xl;
         abil_c ++;
        }

/* Most abilities from items are displaced by 50, so that abilities obtained
naturally or from mutations can be put earlier, to avoid the menu letters
always changing around */

        if (((you[0].equip [7] != -1 && you[0].inv_type [you[0].equip [7]] == 8) || (you[0].equip [8] != -1 && you[0].inv_type [you[0].equip [8]] == 8) || (you[0].equip [1] != -1 && you[0].inv_dam [you[0].equip [1]] % 30 == 6) || (you[0].equip [2] != -1 && you[0].inv_dam [you[0].equip [2]] % 30 == 6) || scan_randarts(13) > 0) && you[0].invis == 0)
        {
                ability [abil_c] = 51;
                ability_fail [abil_c] = 50 - you[0].xl * 2;
                abil_c ++;
        }

        if (you[0].invis != 0)
        {
                ability [abil_c] = 52;
                ability_fail [abil_c] = 0;
                abil_c ++;
        }

        if ((you[0].equip [7] != -1 && you[0].inv_type [you[0].equip [7]] == 18) || (you[0].equip [8] != -1 && you[0].inv_type [you[0].equip [8]] == 18) || (you[0].equip [4] != -1 && you[0].inv_dam [you[0].equip [4]] % 30 == 11) || (scan_randarts(14) > 0) && you[0].lev == 0)
        {
                ability [abil_c] = 53;
                ability_fail [abil_c] = 50 - you[0].xl * 2;
                abil_c ++;
        }

        if (you[0].lev != 0)
        {
                ability [abil_c] = 54;
                ability_fail [abil_c] = 0;
                abil_c ++;
        }

        if (you[0].duration [18] != 0)
        {
                ability [abil_c] = 55;
                ability_fail [abil_c] = 0;
                abil_c ++;
        }


        if ((you[0].equip [7] != -1 && you[0].inv_type [you[0].equip [7]] == 10) || (you[0].equip [8] != -1 && you[0].inv_type [you[0].equip [8]] == 10) || you[0].mutation [27] != 0 || scan_randarts(16) > 0)
        {
                ability [abil_c] = 3;
                ability_fail [abil_c] = 50;
                if (you[0].mutation [27] >= 2) ability_fail [abil_c] = 20;
                abil_c ++;
        }


        abil_c = 26;

        if (you[0].piety >= 30)
        {
                switch(you[0].religion)
                {
                        case 1:
                                ability [abil_c] = 110;
                                ability_fail [abil_c] = 30 - (you[0].piety / 10) - you[0].skills [38] * 6;
                                abil_c ++;
                                break;
                        case 2:
                                ability [abil_c] = 120;
                                ability_fail [abil_c] = 30 - (you[0].piety / 10) - you[0].skills [38] * 6;
                                abil_c ++;
                                break;
                        case 3:
                                ability [abil_c] = 130;
                                ability_fail [abil_c] = 30 - (you[0].piety / 10) - you[0].skills [38] * 6;
                                abil_c ++;
                                break;
                        case 4:
                                ability [abil_c] = 140;
                                ability_fail [abil_c] = 40 - (you[0].piety / 10) - you[0].skills [38] * 3;
                                abil_c ++;
                                break;
                        case 7:
                                ability [abil_c] = 170;
                                ability_fail [abil_c] = 30 - (you[0].piety / 10) - you[0].skills [38] * 6;
                                abil_c ++;
                                break;
                        case 10:
                                ability [abil_c] = 200;
                                ability_fail [abil_c] = 30 - you[0].piety;
                                abil_c ++;
                                break;
                        case 12:
                                ability [abil_c] = 220;
                                ability_fail [abil_c] = 30 - (you[0].piety / 10) - you[0].skills [38] * 6;
                                abil_c ++;
                                break;
                }
        }

        if (you[0].piety >= 50)
        {
                switch(you[0].religion)
                {
                        case 1:
                                ability [abil_c] = 111;
                                ability_fail [abil_c] = 40 - (you[0].piety / 10) - you[0].skills [38] * 5;
                                abil_c ++;
                                break;
                        case 2:
                                ability [abil_c] = 121;
                                ability_fail [abil_c] = 40 - (you[0].piety / 10) - you[0].skills [38] * 5;
                                abil_c ++;
                                break;
                        case 4:
                                ability [abil_c] = 141;
                                ability_fail [abil_c] = 50 - (you[0].piety / 10) - you[0].skills [38] * 4;
                                abil_c ++;
                                break;
                        case 7:
                                ability [abil_c] = 171;
                                ability_fail [abil_c] = 40 - (you[0].piety / 10) - you[0].skills [38] * 5;
                                abil_c ++;
                                break;
                        case 8:
                                ability [abil_c] = 180;
                                ability_fail [abil_c] = 40 - (you[0].piety / 10) - you[0].skills [38] * 5;
                                abil_c ++;
                                break;
                        case 9:
                                ability [abil_c] = 190;
                                ability_fail [abil_c] = 40 - (you[0].piety / 10) - you[0].skills [38] * 5;
                                abil_c ++;
                                break;
                        case 10:
                                ability [abil_c] = 201;
                                ability_fail [abil_c] = 40 - (you[0].piety / 3);
                                abil_c ++;
                                break;
                        case 12:
                                ability [abil_c] = 221;
                                ability_fail [abil_c] = 20 - (you[0].piety / 10) - you[0].skills [38] * 5;
                                abil_c ++;
                                break;
                }
        }

        if (you[0].piety >= 75)
        {
                switch(you[0].religion)
                {
                        case 1:
                                ability [abil_c] = 112;
                                ability_fail [abil_c] = 60 - (you[0].piety / 10) - you[0].skills [38] * 5;
                                abil_c ++;
                                break;
                        case 2:
                                ability [abil_c] = 122;
                                ability_fail [abil_c] = 60 - (you[0].piety / 10) - you[0].skills [38] * 5;
                                abil_c ++;
                                break;
                        case 3:
                                ability [abil_c] = 132;
                                ability_fail [abil_c] = 40 - (you[0].piety / 10) - you[0].skills [38] * 5;
                                abil_c ++;
                                break;
                        case 4:
                                ability [abil_c] = 142;
                                ability_fail [abil_c] = 40 - (you[0].piety / 10) - you[0].skills [38] * 5;
                                abil_c ++;
                                break;
                        case 8:
                                ability [abil_c] = 181;
                                ability_fail [abil_c] = 40 - (you[0].piety / 10) - you[0].skills [38] * 5;
                                abil_c ++;
                                break;
                        case 12:
                                ability [abil_c] = 222;
                                ability_fail [abil_c] = 40 - (you[0].piety / 10) - you[0].skills [38] * 5;
                                abil_c ++;
                                break;
                }
        }

        if (you[0].piety >= 100)
        {
                switch(you[0].religion)
                {
                        case 1:
                                ability [abil_c] = 113;
                                ability_fail [abil_c] = 70 - (you[0].piety / 15) - you[0].skills [38] * 4;
                                abil_c ++;
                                break;
                        case 2:
                                ability [abil_c] = 123;
                                ability_fail [abil_c] = 70 - (you[0].piety / 15) - you[0].skills [38] * 4;
                                abil_c ++;
                                break;
                        case 4:
                                ability [abil_c] = 143;
                                ability_fail [abil_c] = 60 - (you[0].piety / 15) - you[0].skills [38] * 4;
                                abil_c ++;
                                break;
                        case 6:
                                ability [abil_c] = 160;
                                ability_fail [abil_c] = 40 - you[0].intel - you[0].skills [38];
                                abil_c ++;
                                break;
                        case 8:
                                ability [abil_c] = 182;
                                ability_fail [abil_c] = 60 - (you[0].piety / 15) - you[0].skills [38] * 4;
                                abil_c ++;
                                break;
                        case 10:
                                ability [abil_c] = 202;
                                ability_fail [abil_c] = 70 - (you[0].piety / 15) - you[0].skills [38] * 4;
                                abil_c ++;
                                break;
                        case 12:
                                ability [abil_c] = 223;
                                ability_fail [abil_c] = 70 - (you[0].piety / 15) - you[0].skills [38] * 4;
                                abil_c ++;
                                break;
                }
        }

        if (you[0].piety >= 120)
        {
                switch(you[0].religion)
                {
                        case 1:
                                ability [abil_c] = 114;
                                ability_fail [abil_c] = 80 - (you[0].piety / 15) - you[0].skills [38] * 4;
                                abil_c ++;
                                break;
                        case 2:
                                ability [abil_c] = 124;
                                ability_fail [abil_c] = 80 - (you[0].piety / 15) - you[0].skills [38] * 4;
                                abil_c ++;
                                break;
                        case 3:
                                ability [abil_c] = 133;
                                ability_fail [abil_c] = 80 - (you[0].piety / 15) - you[0].skills [38] * 4;
                                abil_c ++;
                                break;
                        case 4:
                                ability [abil_c] = 144;
                                ability_fail [abil_c] = 70 - (you[0].piety / 15) - you[0].skills [38] * 4;
                                abil_c ++;
                                break;
                        case 7:
                                ability [abil_c] = 172;
                                ability_fail [abil_c] = 70 - (you[0].piety / 15) - you[0].skills [38] * 4;
                                abil_c ++;
                                break;
                        case 8:
                                ability [abil_c] = 183;
                                ability_fail [abil_c] = 70 - (you[0].piety / 15) - you[0].skills [38] * 4;
                                abil_c ++;
                                break;
                        case 12:
                                ability [abil_c] = 224;
                                ability_fail [abil_c] = 80 - (you[0].piety / 15) - you[0].skills [38] * 4;
                                abil_c ++;
                                break;
                }
        }


        if (you[0].religion != 0)
        {
                ability [49] = 250;
                ability_fail [49] = 0;
        }


        for (i = 0; i < 120; i ++)
        {
                if (ability [i] != 0) break;
                if (i == 119)
                {
                        strcpy(info, "Sorry, you're not good enough to have a special ability.");
                        mpr(info);
                        return;
                }
        }


/*
if (you[0].hung_state <= 2)
{
 strcpy(info, "You're too hungry.");
 mpr(info);
 return; // no you[0].turnover?
}

if (you[0].duration [17] != 0)
{
 strcpy(info, "You can't do that yet.");
 mpr(info);
 return; // no you[0].turnover?
}
*/
        query :
        strcpy(info, "Use which ability? (? or * to list)");
        mpr(info);

        int keyin = get_ch();

        if (keyin == '?' || keyin == '*')
        {
                char unthing = show_abilities(ability, ability_fail); //you[0].spells); //invent(0, you[0].inv_quant, you[0].inv_dam, you[0].inv_class, you[0].inv_type, you[0].inv_plus, you[0].inv_ident, you[0].equip [0], you[0].equip [6], you[0].equip [5], you[0].equip [2], you[0].equip [1], you[0].equip [3], you[0].equip [4], you[0].ring);
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

        if ((spc < 97 || spc > 121) && (spc < 65 || spc > 90))
        {
                unknown : strcpy(info, "You can't do that.");
                mpr(info);
                return;
        }

        spc2 = conv_lett(spc);

        if (ability [spc2] == 0)
        {
                goto unknown;
        }

        abil_used = spc2;

        if (random2(33) + random2(33) + random2(34) <= ability_fail [abil_used])
        {
                mpr("You fail to use your ability.");
                you[0].turnover = 1;
                return;
        }

    you[0].turnover = 1;

        you[0].ep_ch = 1;

        switch(ability [abil_used])
        {
                case 1: // Naga, + spit poison mutation
                        if (you[0].duration [17] != 0)
                        {
                                mpr("You can't do that yet.");
                                return;
                        }
                        if (you[0].hung_state <= 2)
                        {
                                strcpy(info, "You're too hungry.");
                                mpr(info);
                                return;
                        }
                        if (spell_direction(abild, beam) == -1)
                        {
                                strcpy(info, "Okay, then.");
                                mpr(info);
                                return;
                        }
                        strcpy(info, "You spit poison.");
                        mpr(info);
                        you[0].turnover = 1;
                        you[0].duration [17] += random2(5) + 3;
                        zapping(36, you[0].xl + you[0].mutation [28] * 5, beam);
                        you[0].hunger -= 40 + random2(40) + random2(40);
                        strcpy(info, "You feel slightly more hungry.");
                        mpr(info);
                        break;

                case 2: // Gnome + sense surrounds mut
                        strcpy(info, "You sense your surroundings.");
                        mpr(info);
// you[0].duration [17] += 30 - you[0].xl + random2(40 - you[0].xl) + random2(5);
                        magic_mapping(3 + random2(you[0].xl) + random2(you[0].xl) + you[0].mutation [29] * 10, 40 + random2(you[0].xl) + random2(you[0].xl) + random2(you[0].xl));
                        you[0].hunger -= 30 + random2(30) + random2(30);
                        you[0].turnover = 1;
                        break;

                case 3: // ring of teleport, + teleport mut
                        if (you[0].hung_state <= 2)
                        {
                                strcpy(info, "You're too hungry.");
                                mpr(info);
                                return;
                        }
                        if (you[0].ep < 3)
                        {
                                mpr("You don't have enough magic to use that ability.");
                                return;
                        }
                        you[0].ep -= 3;
                        you[0].ep_ch = 1;
                        you[0].hunger -= 200 + random2(200) + random2(200);
                        if (you[0].mutation [27] == 3) you_teleport2(1);
                        else you_teleport();
                        break;

                case 4:
                case 6:
                case 7:
                case 8:
                case 9:
                case 10:
                case 12:
                case 13: /* Breathe things */
                        if (you[0].duration [17] != 0 && ability [abil_used] != 9)
                        {
                                mpr("You can't do that yet.");
                                return;
                        }
                        if (you[0].hung_state <= 2)
                        {
                                mpr("You're too hungry.");
                                return;
                        }
                        if (spell_direction(abild, beam) == -1)
                        {
                                mpr("Okay, then.");
                                return;
                        }
                        switch(ability [abil_used])
                        {
                                case 4: mpr("You breathe fire.");
                                        zapping(38, you[0].xl + you[0].mutation [28] * 4 + (you[0].attribute [5] == 5) * 10, beam); break;
                                case 6: mpr("You breathe a wave of freezing cold.");
                                        zapping(39, you[0].xl, beam); break;
                                case 7: mpr("You breathe a blast of poison gas.");
                                        zapping(41, you[0].xl, beam); break;
                                case 8: mpr("You spit a bolt of lightning.");
                                        zapping(14, you[0].xl * 2, beam); break;
                                case 9: mpr("You spit acid.");
                                        zapping(40, you[0].xl, beam); break;
                                case 10: mpr("You spit a bolt of incandescent energy.");
                                        zapping(42, you[0].xl, beam); break;
                case 12: mpr("You spit a glob of burning liquid.");
                                        zapping(23, you[0].xl, beam); break;
                            case 13: mpr("You breathe a blast of scalding steam.");
                                        zapping(48, you[0].xl, beam); break;
                }
            you[0].turnover = 1;
                        if (ability [abil_used] != 9) you[0].duration [17] += random2(5) + 3 + random2(30 - you[0].xl);
            if (ability [abil_used] == 13) you[0].duration [17] /= 2;
                        you[0].hunger -= 200 + random2(100) + random2(100) - you[0].xl * random2(5);
                        strcpy(info, "You feel slightly more hungry.");
                        mpr(info);
                        break;

                case 5: // blink mut
                        if (you[0].hung_state <= 2)
                        {
                                strcpy(info, "You're too hungry.");
                                mpr(info);
                                return;
                        }
                        if (you[0].ep < 1)
                        {
                                mpr("You don't have enough magic to use that ability.");
                                return;
                        }
                        you[0].ep -= 1;
                        you[0].ep_ch = 1;
                        you[0].hunger -= 50 + random2(50) + random2(50);
                        random_blink();
                        break;

                case 11: // berserk
                        if (you[0].hung_state <= 2)
                        {
                                strcpy(info, "You're too hungry.");
                                mpr(info);
                                return;
                        }
                        if (go_berserk() == 0) mpr("You fail to go berserk.");
                        break;

                case 14: // Fly (kenku). Eventually becomes permanent (handled in acr.cc)
                        if (you[0].hung_state <= 2)
                        {
                                strcpy(info, "You're too hungry.");
                                mpr(info);
                                return;
                        }
                        if (you[0].ep < 3)
                        {
                                mpr("You don't have enough magic to use that ability.");
                                return;
                        }
                        you[0].ep -= 3;
                        you[0].ep_ch = 1;
                        you[0].hunger -= 100 + random2(100) + random2(100);
            cast_fly(you[0].xl * 4);
            if (you[0].xl >= 15)
            {
              mpr("You feel very comfortable in the air.");
              you[0].lev = 100;
              you[0].duration [12] = 100;
            }
                        break;

/* Demonic powers follow: */

                case 15:  /* summon minor demon */
                        if (you[0].hung_state <= 2)
                        {
                                mpr("You're too hungry.");
                                return;
                        }
                        if (you[0].ep < 3)
                        {
                                mpr("You don't have enough magic to use that ability.");
                                return;
                        }
                        if (you[0].hp <= 3)
                        {
                                mpr("You don't have enough vitality to use that power.");
                                return;
                        }
                        you[0].ep -= 3;
                        you[0].ep_ch = 1;
                        you[0].hp -= 3;
                        you[0].hp_ch = 1;
                        you[0].hunger -= 50 + random2(100) + random2(100);
                        summon_ice_beast_etc(you[0].xl * 4, summon_any_demon(0));
                        break;

                case 16:  /* summon demon */
                        if (you[0].hung_state <= 2)
                        {
                                mpr("You're too hungry.");
                                return;
                        }
                        if (you[0].ep < 5)
                        {
                                mpr("You don't have enough magic to use that ability.");
                                return;
                        }
                        if (you[0].hp <= 5)
                        {
                                mpr("You don't have enough vitality to use that power.");
                                return;
                        }
                        you[0].ep -= 5;
                        you[0].ep_ch = 1;
                        you[0].hp -= 5;
                        you[0].hp_ch = 1;
                        you[0].hunger -= 100 + random2(200) + random2(200);
                        summon_ice_beast_etc(you[0].xl * 4, summon_any_demon(1));
                        break;

                case 17:  /* Hellfire */
                        if (you[0].hung_state <= 2)
                        {
                                mpr("You're too hungry.");
                                return;
                        }
                        if (you[0].ep < 8)
                        {
                                mpr("You don't have enough magic to use that ability.");
                                return;
                        }
                        if (you[0].hp <= 8)
                        {
                                mpr("You don't have enough vitality to use that power.");
                                return;
                        }
                        you[0].ep -= 8;
                        you[0].ep_ch = 1;
                        you[0].hp -= 8;
                        you[0].hp_ch = 1;
                        you[0].hunger -= 200 + random2(300) + random2(300);
            your_spells(118, 100, 0); // power (2nd number) is meaningless
                        break;

                case 18:  /* Torment */
                        if (you[0].is_undead != 0)
                        {
                                mpr("Undead cannot use this ability.");
                                return;
                        }
                        if (you[0].hung_state <= 2)
                        {
                                mpr("You're too hungry.");
                                return;
                        }
                        if (you[0].ep < 9)
                        {
                                mpr("You don't have enough magic to use that ability.");
                                return;
                        }
                        you[0].ep -= 9;
                        you[0].ep_ch = 1;
                        you[0].hunger -= 300 + random2(300) + random2(300);
            torment();
                        break;

                case 19:  /* Raise dead */
                        if (you[0].hung_state <= 2)
                        {
                                mpr("You're too hungry.");
                                return;
                        }
                        if (you[0].ep < 5)
                        {
                                mpr("You don't have enough magic to use that ability.");
                                return;
                        }
                        if (you[0].hp <= 5)
                        {
                                mpr("You don't have enough vitality to use that power.");
                                return;
                        }
                        you[0].ep -= 5;
                        you[0].ep_ch = 1;
                        you[0].hp -= 5;
                        you[0].hp_ch = 1;
                        you[0].hunger -= 100 + random2(200) + random2(200);
            your_spells(66, you[0].xl * 5, 0); // power (2nd number) is meaningless
                        break;

                case 20:  /* Control demon */
                        if (you[0].hung_state <= 2)
                        {
                                mpr("You're too hungry.");
                                return;
                        }
                        if (you[0].ep < 4)
                        {
                                mpr("You don't have enough magic to use that ability.");
                                return;
                        }
                        if (you[0].hp <= 4)
                        {
                                mpr("You don't have enough vitality to use that power.");
                                return;
                        }
                        if (spell_direction(abild, beam) == -1)
                        {
                                mpr("Okay, then.");
                                return;
                        }
                        you[0].ep -= 4;
                        you[0].ep_ch = 1;
                        you[0].hp -= 4;
                        you[0].hp_ch = 1;
                        you[0].hunger -= 100 + random2(100) + random2(100);
                        zapping(49, you[0].xl * 5, beam);
                        break;

                case 21:  /* Pandemonium */
            if (you[0].level_type == 3)
            {
                                mpr("You're already here.");
                                return;
            }
                        if (you[0].hung_state <= 2)
                        {
                                mpr("You're too hungry.");
                                return;
                        }
                        if (you[0].ep < 4)
                        {
                                mpr("You don't have enough magic to use that ability.");
                                return;
                        }
                        you[0].ep -= 7;
                        you[0].ep_ch = 1;
                        you[0].hunger -= 200 + random2(200) + random2(200);
            banished(99);
                        break;

                case 22:  /* Channeling */
                        if (you[0].hung_state <= 2)
                        {
                                mpr("You're too hungry.");
                                return;
                        }
                        if (you[0].hp < 1)
                        {
                                mpr("You don't have enough vitality to use that power.");
                                return;
                        }
                        you[0].hp -= 1;
                        you[0].hp_ch = 1;
                        you[0].hunger -= 100 + random2(100) + random2(100);
                    you[0].ep += 1 + random2(5);
                    if (you[0].ep > you[0].ep_max) you[0].ep = you[0].ep_max;
                    you[0].ep_ch = 1;
                    mpr("You channel some magical energy.");
                        break;

        case 23:
                case 24:  /* Throw flame/frost */
                        if (you[0].hung_state <= 2)
                        {
                                mpr("You're too hungry.");
                                return;
                        }
                        if (you[0].ep < 1)
                        {
                                mpr("You don't have enough magic to use that ability.");
                                return;
                        }
                        if (you[0].hp <= 1)
                        {
                                mpr("You don't have enough vitality to use that power.");
                                return;
                        }
                        if (spell_direction(abild, beam) == -1)
                        {
                                mpr("Okay, then.");
                                return;
                        }
                        you[0].ep -= 1;
                        you[0].ep_ch = 1;
                        you[0].hp -= 1;
                        you[0].hp_ch = 1;
                        you[0].hunger -= 50 + random2(100) + random2(100);
                        if (ability [abil_used] == 23) zapping(0, you[0].xl * 3, beam);
             else zapping(1, you[0].xl * 3, beam);
                        break;

                case 25:  /* bolt of draining */
                        if (you[0].hung_state <= 2)
                        {
                                mpr("You're too hungry.");
                                return;
                        }
                        if (you[0].ep < 4)
                        {
                                mpr("You don't have enough magic to use that ability.");
                                return;
                        }
                        if (you[0].hp <= 4)
                        {
                                mpr("You don't have enough vitality to use that power.");
                                return;
                        }
                        if (spell_direction(abild, beam) == -1)
                        {
                                mpr("Okay, then.");
                                return;
                        }
                        you[0].ep -= 4;
                        you[0].ep_ch = 1;
                        you[0].hp -= 4;
                        you[0].hp_ch = 1;
                        you[0].hunger -= 100 + random2(100) + random2(100);
                        zapping(17, you[0].xl * 6, beam);
                        break;

/* Items etc */
                case 51: /* turn invis */
                        if (you[0].hung_state <= 2)
                        {
                                mpr("You're too hungry.");
                                return;
                        }
                        if (you[0].ep < 2)
                        {
                                mpr("You don't have enough magic to use that ability.");
                                return;
                        }
                        you[0].ep -= 2;
                        you[0].ep_ch = 1;
                        you[0].hunger -= 250 + random2(250) + random2(250);
                        potion_effect(12, you[0].xl + 10);
                        break;

                case 52: /* uninvisible */
            mpr("You feel less transparent.");
                        you[0].invis = 1;
                        break;

                case 53: /* levitate */
                        if (you[0].hung_state <= 2)
                        {
                                mpr("You're too hungry.");
                                return;
                        }
                        if (you[0].ep < 1)
                        {
                                mpr("You don't have enough magic to use that ability.");
                                return;
                        }
                        you[0].ep -= 1;
                        you[0].ep_ch = 1;
                        you[0].hunger -= 100 + random2(100) + random2(100);
                        potion_effect(7, you[0].xl + 10);
                        break;

                case 54: /* unlevitate */
            mpr("You feel heavy.");
                        you[0].lev = 1;
                        break;

                case 55: /* untransform */
            mpr("You feel almost normal.");
                        you[0].duration [18] = 2;
                        break;

                case 110:
                case 120:
                        if (you[0].hung_state <= 2)
                        {
                                mpr("You're too hungry.");
                                return;
                        }
                        if (you[0].     ep < 1)
                        {
                                mpr("You don't have enough magic to use that ability.");
                                return;
                        }
                        you[0].ep -= 1;
                        you[0].ep_ch = 1;
                        turn_undead(you[0].piety);
            exercise(38, 1);
                        break;

                case 111: /* Minor Healing */
                        if (you[0].hung_state <= 2)
                        {
                                mpr("You're too hungry.");
                                return;
                        }
                        if (you[0].     ep < 2)
                        {
                                mpr("You don't have enough magic to use that ability.");
                                return;
                        }
                        you[0].ep -= 2;
                        you[0].ep_ch = 1;
                        if (cast_lesser_healing() == 0) break;
                        you[0].hunger -= 150 + random2(150) + random2(150);
                        lose_piety(1);
            exercise(38, 1 + random2(3));
                        break;

                case 112: /* Pestilence */
                        if (you[0].hung_state <= 2)
                        {
                                mpr("You're too hungry.");
                                return;
                        }
                        if (you[0].ep < 3)
                        {
                                mpr("You don't have enough magic to use that ability.");
                                return;
                        }
                        you[0].ep -= 3;
                        you[0].ep_ch = 1;
                        you[0].hunger -= 350 + random2(350) + random2(350);
                        summon_swarm(you[0].piety);
                        lose_piety(2 + random2(2));
            exercise(38, 2 + random2(4));
                        break;

                case 113: /* Holy Word */
                        if (you[0].hung_state <= 2)
                        {
                                mpr("You're too hungry.");
                                return;
                        }
                        if (you[0].     ep < 6)
                        {
                                mpr("You don't have enough magic to use that ability.");
                                return;
                        }
                        you[0].ep -= 6;
                        you[0].ep_ch = 1;
                        you[0].hunger -= 350 + random2(350) + random2(350);
                        holy_word(you[0].piety);
                        lose_piety(3 + random2(2));
            exercise(38, 3 + random2(5));
                        break;

                case 114:  /* Guardian */
                        if (you[0].hung_state <= 2)
                        {
                                mpr("You're too hungry.");
                                return;
                        }
                        if (you[0].ep < 4)
                        {
                                mpr("You don't have enough magic to use that ability.");
                                return;
                        }
                        you[0].ep -= 4;
                        you[0].ep_ch = 1;
                        you[0].hunger -= 350 + random2(350) + random2(350);
                        summon_ice_beast_etc(you[0].skills [38] * 4, 26);
                        lose_piety(3 + random2(4));
            exercise(38, 8 + random2(10));
                        break;




                case 121:
                        if (you[0].hung_state <= 2)
                        {
                                mpr("You're too hungry.");
                                return;
                        }
                        if (you[0].ep < 3)
                        {
                                mpr("You don't have enough magic to use that ability.");
                                return;
                        }
                        you[0].ep -= 3;
                        you[0].ep_ch = 1;
                        you[0].hunger -= 150 + random2(150) + random2(150);
                        cast_smiting(you[0].skills [38] * 8);
                        lose_piety(1);
            exercise(38, 2 + random2(2));
                        break;

                case 122:  /* destroy undead */
                        if (you[0].hung_state <= 2)
                        {
                                mpr("You're too hungry.");
                                return;
                        }
                        if (you[0].ep < 3)
                        {
                                mpr("You don't have enough magic to use that ability.");
                                return;
                        }
                        if (spell_direction(spd, beam) == -1)
                        {
                                mpr("Okay, then.");
                        }
                        you[0].ep -= 3;
                        you[0].ep_ch = 1;
                        you[0].hunger -= 150 + random2(250) + random2(250);
                        zapping(24, you[0].skills [38] * 6, beam);
                        lose_piety(1 + random2(2));
            exercise(38, 2 + random2(4));
                        break;

                case 123:  /* thunderbolt */
                        if (you[0].hung_state <= 2)
                        {
                                mpr("You're too hungry.");
                                return;
                        }
                        if (you[0].ep < 5)
                        {
                                mpr("You don't have enough magic to use that ability.");
                                return;
                        }
                        if (spell_direction(spd, beam) == -1)
                        {
                                mpr("Okay, then.");
                        }
                        you[0].ep -= 5;
                        you[0].ep_ch = 1;
                        you[0].hunger -= 350 + random2(350) + random2(350);
                        zapping(14, you[0].skills [38] * 6, beam);
                        lose_piety(1 + random2(2));
            exercise(38, 3 + random2(6));
                        break;

                case 124:  /* Daeva */
                        if (you[0].hung_state <= 2)
                        {
                                mpr("You're too hungry.");
                                return;
                        }
                        if (you[0].ep < 5)
                        {
                                mpr("You don't have enough magic to use that ability.");
                                return;
                        }
                        you[0].ep -= 5;
                        you[0].ep_ch = 1;
                        you[0].hunger -= 350 + random2(350) + random2(350);
                        summon_ice_beast_etc(you[0].skills [38] * 4, 366);
                        lose_piety(3 + random2(4));
            exercise(38, 8 + random2(10));
                        break;


                case 130:  /* recall undead */
                        if (you[0].hung_state <= 2)
                        {
                                mpr("You're too hungry.");
                                return;
                        }
                        if (you[0].ep < 1)
                        {
                                mpr("You don't have enough magic to use that ability.");
                                return;
                        }
                        you[0].ep -= 1;
                        you[0].ep_ch = 1;
                        recall(1);
            exercise(38, 1);
                        break;

                case 132:  /* enslave undead */
                        if (you[0].hung_state <= 2)
                        {
                                mpr("You're too hungry.");
                                return;
                        }
                        if (you[0].ep < 4)
                        {
                                mpr("You don't have enough magic to use that ability.");
                                return;
                        }
                        if (spell_direction(spd, beam) == -1)
                        {
                                mpr("Okay, then.");
                        }
                        you[0].ep -= 4;
                        you[0].ep_ch = 1;
                        you[0].hunger -= 350 + random2(350) + random2(350);
                        zapping(43, you[0].skills [38] * 8, beam);
                        lose_piety(2 + random2(2));
            exercise(38, 5 + random2(5));
                        break;

                case 133:  /* Invoke Death */
                        if (you[0].hung_state <= 2)
                        {
                                mpr("You're too hungry.");
                                return;
                        }
                        if (you[0].ep < 4)
                        {
                                mpr("You don't have enough magic to use that ability.");
                                return;
                        }
                        you[0].ep -= 4;
                        you[0].ep_ch = 1;
                        you[0].hunger -= 350 + random2(350) + random2(350);
                        summon_ice_beast_etc(20 + you[0].skills [38] * 3, 83);
                        lose_piety(3 + random2(4));
            exercise(38, 10 + random2(14));
                        break;


                case 140:  /* Animate Skeleton */
                        if (you[0].hung_state <= 2)
                        {
                                mpr("You're too hungry.");
                                return;
                        }
                        if (you[0].ep < 3)
                        {
                                mpr("You don't have enough magic to use that ability.");
                                return;
                        }
                        you[0].ep -= 3;
                        you[0].ep_ch = 1;
                        you[0].hunger -= 100 + random2(100) + random2(100);
            animate_a_corpse(you[0].x_pos, you[0].y_pos, 7, you[0].pet_target, 0);
            exercise(38, 2 + random2(4));
                        break;

                case 141:  /* Recall Undead */
                        if (you[0].hung_state <= 2)
                        {
                                mpr("You're too hungry.");
                                return;
                        }
                        if (you[0].ep < 4)
                        {
                                mpr("You don't have enough magic to use that ability.");
                                return;
                        }
                        you[0].ep -= 4;
                        you[0].ep_ch = 1;
                        you[0].hunger -= 150 + random2(150) + random2(150);
            recall(1);
            exercise(38, 2 + random2(4));
                        break;

                case 142:  /* Animate Dead */
                        if (you[0].hung_state <= 2)
                        {
                                mpr("You're too hungry.");
                                return;
                        }
                        if (you[0].ep < 7)
                        {
                                mpr("You don't have enough magic to use that ability.");
                                return;
                        }
                        you[0].ep -= 7;
                        you[0].ep_ch = 1;
                        you[0].hunger -= 250 + random2(250) + random2(250);
            animate_dead(you[0].skills [38] + 1, 7, you[0].pet_target, 1);
            exercise(38, 2 + random2(4));
                        lose_piety(1 + random2(2));
                        break;

                case 143:  /* Drain Life */
                        if (you[0].hung_state <= 2)
                        {
                                mpr("You're too hungry.");
                                return;
                        }
                        if (you[0].ep < 6)
                        {
                                mpr("You don't have enough magic to use that ability.");
                                return;
                        }
                        you[0].ep -= 6;
                        you[0].ep_ch = 1;
                        you[0].hunger -= 250 + random2(250) + random2(250);
                        drain_life(you[0].skills [38]);
            exercise(38, 2 + random2(4));
                        lose_piety(1 + random2(3));
                        break;

                case 144:  /* Control Undead */
                        if (you[0].hung_state <= 2)
                        {
                                mpr("You're too hungry.");
                                return;
                        }
                        if (you[0].ep < 5)
                        {
                                mpr("You don't have enough magic to use that ability.");
                                return;
                        }
                        you[0].ep -= 5;
                        you[0].ep_ch = 1;
                        you[0].hunger -= 250 + random2(250) + random2(250);
            mass_enchantment(30, you[0].skills [38] * 8);
            exercise(38, 3 + random2(4));
                        lose_piety(2 + random2(3));
                        break;

                case 160: /* channeling */
                        if (you[0].hung_state <= 2)
                        {
                                strcpy(info, "You're too hungry.");
                                mpr(info);
                                return;
                        }
                        you[0].hunger -= 50 + random2(50) + random2(50);
                    you[0].ep += 1 + random2(you[0].skills [38] / 4 + 2);
                    if (you[0].ep > you[0].ep_max) you[0].ep = you[0].ep_max;
                    you[0].ep_ch = 1;
                    strcpy(info, "You channel some magical energy.");
                    mpr(info);
            exercise(38, 1 + random2(3));
                        break;

                case 170: /* might */
                        if (you[0].hung_state <= 2)
                        {
                                strcpy(info, "You're too hungry.");
                                mpr(info);
                                return;
                        }
                        if (you[0].ep < 2)
                        {
                                mpr("You don't have enough magic to use that ability.");
                                return;
                        }
                        you[0].ep -= 2;
                        potion_effect(3, you[0].skills [38] * 8);
                        lose_piety(1);
                        you[0].hunger -= 200 + random2(200) + random2(200);
            exercise(38, 1 + random2(3));
                        break;

                case 171: /* Minor healing */
                        if (you[0].hung_state <= 2)
                        {
                                mpr("You're too hungry.");
                                return;
                        }
                        if (you[0].ep < 2)
                        {
                                mpr("You don't have enough magic to use that ability.");
                                return;
                        }
                        you[0].ep -= 2;
                        if (cast_lesser_healing() == 0) break;
                        lose_piety(1);
                        you[0].hunger -= 100 + random2(100) + random2(100);
            exercise(38, 2 + random2(5));
                        break;


                case 172: /* haste */
                        if (you[0].hung_state <= 2)
                        {
                                strcpy(info, "You're too hungry.");
                                mpr(info);
                                return;
                        }
                        if (you[0].ep < 5)
                        {
                                mpr("You don't have enough magic to use that ability.");
                                return;
                        }
                        you[0].ep -= 5;
                        potion_effect(2, you[0].skills [38] * 8);
                        lose_piety(2 + random2(3));
                        you[0].hunger -= 300 + random2(300) + random2(300);
            exercise(38, 3 + random2(7));
                        break;




                case 180:  /* minor destr */
                        if (you[0].hung_state <= 2)
                        {
                                mpr("You're too hungry.");
                                return;
                        }
                        if (you[0].ep < 1)
                        {
                                mpr("You don't have enough magic to use that ability.");
                                return;
                        }
                        if (spell_direction(spd, beam) == -1)
                        {
                                mpr("Okay, then.");
                        }
                        you[0].ep -= 1;
                        you[0].ep_ch = 1;
                        you[0].hunger -= 100 + random2(150) + random2(150);
                        switch(random2(5))
                        {
                                case 0: zapping(0, you[0].skills [38] * 6, beam); break;
                                case 1: zapping(22, you[0].skills [38] * 6, beam); break;
                                case 2: zapping(33, you[0].skills [38] * 6, beam); break;
                                case 3: zapping(34, you[0].skills [38] * 6, beam); break;
                                case 4: zapping(40, you[0].skills [38] * 6, beam); break;
                        }
            exercise(38, 1 + random2(2));
                        break;

                case 181:  /* minor summoning */
                        if (you[0].hung_state <= 2)
                        {
                                mpr("You're too hungry.");
                                return;
                        }
                        if (you[0].ep < 2)
                        {
                                mpr("You don't have enough magic to use that ability.");
                                return;
                        }
                        you[0].ep -= 2;
                        you[0].ep_ch = 1;
                        you[0].hunger -= 350 + random2(350) + random2(350);
                        summon_ice_beast_etc(20 + you[0].skills [38] * 3, 225 + random2(5));
                        lose_piety(1 + random2(2));
            exercise(38, 2 + random2(3));
                        break;

                case 182:  /* major destr */
                        if (you[0].hung_state <= 2)
                        {
                                mpr("You're too hungry.");
                                return;
                        }
                        if (you[0].ep < 3)
                        {
                                mpr("You don't have enough magic to use that ability.");
                                return;
                        }
                        if (spell_direction(spd, beam) == -1)
                        {
                                mpr("Okay, then.");
                        }
                        you[0].ep -= 3;
                        you[0].ep_ch = 1;
                        you[0].hunger -= 300 + random2(350) + random2(350);
                        switch(random2(8))
                        {
                                case 0: zapping(7, you[0].skills [38] * 5, beam); break;
                                case 1: zapping(12, you[0].skills [38] * 5, beam); break;
                                case 2: zapping(14, you[0].skills [38] * 5, beam); break;
                                case 3: zapping(17, you[0].skills [38] * 5, beam); break;
                                case 4: zapping(23, you[0].skills [38] * 5, beam); break;
                                case 5: zapping(31, you[0].skills [38] * 5, beam); break;
                                case 6: zapping(35, you[0].skills [38] * 5, beam); break;
                                case 7: you[0].attribute [0] ++;
                                        mpr("Makhleb hurls a blast of lightning!");
                                        beam[0].beam_source = MNG;
                                        beam[0].type = 43;
                                        beam[0].damage = 130;
                                        beam[0].flavour = 5;
                                        beam[0].bx = you[0].x_pos;
                                        beam[0].by = you[0].y_pos;
                                        strcpy(beam[0].beam_name, "blast of lightning");
                                        beam[0].colour = LIGHTCYAN;
                                        beam[0].thing_thrown = 1; /* your expl */
                                        explosion(1, beam);
                                        you[0].attribute [0] --;
                                        break;
                        }
                        lose_piety(1 + random2(2));
            exercise(38, 3 + random2(5));
                        break;

                case 183:  /* major summoning */
                        if (you[0].hung_state <= 2)
                        {
                                mpr("You're too hungry.");
                                return;
                        }
                        if (you[0].ep < 4)
                        {
                                mpr("You don't have enough magic to use that ability.");
                                return;
                        }
                        you[0].ep -= 4;
                        you[0].ep_ch = 1;
                        you[0].hunger -= 500 + random2(450) + random2(450);
                        summon_ice_beast_etc(20 + you[0].skills [38] * 3, 230 + random2(5));
                        lose_piety(2 + random2(2));
            exercise(38, 6 + random2(6));
                        break;



                case 200: /* berserk */
                        if (you[0].hung_state <= 2)
                        {
                                strcpy(info, "You're too hungry.");
                                mpr(info);
                                return;
                        }
                        if (go_berserk() == 0) mpr("You fail to go berserk.");
                        you[0].hunger -= 100 + random2(100) + random2(100);
            //exercise(38, 1 + random2(3));
                        break;

                case 201: /* might */
                        if (you[0].hung_state <= 2)
                        {
                                strcpy(info, "You're too hungry.");
                                mpr(info);
                                return;
                        }
                        potion_effect(3, you[0].skills [38] * 6);
                        lose_piety(1 + random2(2));
                        you[0].hunger -= 200 + random2(200) + random2(200);
            //exercise(38, 2 + random2(5));
                        break;

                case 202: /* haste */
                        if (you[0].hung_state <= 2)
                        {
                                strcpy(info, "You're too hungry.");
                                mpr(info);
                                return;
                        }
                        potion_effect(2, you[0].skills [38] * 6);
                        lose_piety(2 + random2(3));
                        you[0].hunger -= 300 + random2(300) + random2(300);
            //exercise(38, 4 + random2(6));
                        break;




                case 190:
                        cast_selective_amnesia();
            //exercise(38, 1);
                        break;


                case 220:
                        if (you[0].hung_state <= 2)
                        {
                                mpr("You're too hungry.");
                                return;
                        }
                        if (you[0].ep < 1)
                        {
                                mpr("You don't have enough magic to use that ability.");
                                return;
                        }
                        you[0].ep -= 1;
                        you[0].ep_ch = 1;
                        if (cast_lesser_healing() == 0) break;
                        you[0].hunger -= 100 + random2(100) + random2(100);
            exercise(38, 1 + random2(2));
                        break;

                case 221:
                        if (you[0].hung_state <= 2)
                        {
                                mpr("You're too hungry.");
                                return;
                        }
                        if (you[0].ep < 1)
                        {
                                mpr("You don't have enough magic to use that ability.");
                                return;
                        }
                        you[0].ep -= 1;
                        you[0].ep_ch = 1;
                        lose_piety(1);
                        you[0].hunger -= 150 + random2(150) + random2(150);
                        purification();
            exercise(38, 2 + random2(3));
                        break;

                case 222:
                        if (you[0].hung_state <= 2)
                        {
                                mpr("You're too hungry.");
                                return;
                        }
                        if (you[0].ep < 2)
                        {
                                mpr("You don't have enough magic to use that ability.");
                                return;
                        }
                        if (cast_greater_healing() == 0) break;
                        you[0].ep -= 2;
                        you[0].ep_ch = 1;
                        you[0].hunger -= 250 + random2(250) + random2(250);
                        lose_piety(1 + random2(3));
            exercise(38, 3 + random2(5));
                        break;

                case 223:
                        if (you[0].hung_state <= 2)
                        {
                                mpr("You're too hungry.");
                                return;
                        }
                        if (you[0].ep < 3)
                        {
                                mpr("You don't have enough magic to use that ability.");
                                return;
                        }
                        you[0].ep -= 3;
                        you[0].ep_ch = 1;
                        you[0].hunger -= 400 + random2(450) + random2(450);
                    strcpy(info, "You feel refreshed."); // ...can't think of anything better right now
                        mpr(info);
                        restore_str();
                        restore_int();
                        restore_dex();
            if (you[0].base_hp < 5000) you[0].base_hp = 5000;
            calc_hp();
                        lose_piety(2 + random2(3));
            exercise(38, 4 + random2(6));
                        break;

                case 224:
                        if (you[0].hung_state <= 2)
                        {
                                mpr("You're too hungry.");
                                return;
                        }
                        if (you[0].ep < 6)
                        {
                                mpr("You don't have enough magic to use that ability.");
                                return;
                        }
                        if (cast_greatest_healing() == 0) break;
                        you[0].ep -= 6;
                        you[0].ep_ch = 1;
                        you[0].hunger -= 600 + random2(550) + random2(550);
//cast_purification(you[0].piety);
                        lose_piety(4 + random2(4));
            exercise(38, 6 + random2(10));
                        break;


                case 250: /* give up religion */
                        mpr("Really give up your religion, and lose all of its fabulous benefits?");
                        keyi = get_ch();
                        if (keyi != 'y' && keyi != 'Y')
                        {
                                mpr("Okay, then.");
                                break;
                        }
                        mpr("Are you absolutely sure?");
                        keyi = get_ch();
                        if (keyi != 'y' && keyi != 'Y')
                        {
                                mpr("Okay, then.");
                                break;
                        }
                        excommunication();
                        break;

                default:
                        strcpy(info, "Sorry, you can't do that.");
                        mpr(info);
                        return;
        }

}
/*
Lists any abilities the player may have.
*/
char show_abilities(int ability [120], int ability_fail [120])
{
        int j;
        char lines = 0;
        unsigned char anything = 0;
        char strng [5] = "";
        char ft;
        char ki;
        char already = 0;
        char is_invocations = 0;

        for (j = 0; j < 120; j ++)
        {
        if (ability [j] >= 100) is_invocations = 1;
        }


#ifdef DOS_TERM
        char buffer[4800];
        gettext(1, 1, 80, 25, buffer);
        window(1, 1, 80, 25);
#endif

        clrscr();

        cprintf("  Ability                           Cost                    Success");
        lines++;

        if (lines > 22)
        {
                gotoxy(1,25);
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
                if (ki == 0) ki = getch();
                lines = 0;
                clrscr();
                gotoxy(1,1);
                anything = 0;

        }
        if (lines > 1) cprintf(EOL" ");

        for (j = 0; j < 120; j++)
        {

                if (lines > 23)
                {
                        gotoxy(1,25);
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
                        if (ki == 0) ki = getch();
                        lines = 0;
                        clrscr();
                        gotoxy(1,1);
                        anything = 0;
                }

                if (j == 25 && is_invocations == 1)
                {
                anything++;
                        textcolor(BLUE);
                        cprintf(EOL"    Invocations - ");
                        textcolor(LIGHTGREY);
                        lines++;
                        continue;
                }

                if (ability [j] != 0)
                {

                        anything ++;
                        if (lines > 0) cprintf(EOL);
                        lines++;
                        cprintf(" ");
                        if (j < 26) ft = (char) j + 97;
                        else ft = (char) j + 65 - 26;
                        strng [0] = ft;
                        cprintf(strng);
                        cprintf(" - ");

                        switch(ability [j])
                        {
                                case 1:
                                        cprintf("Spit Poison                     Food");
                                        break;
                                case 2:
                                        cprintf("Sense Surroundings              Food");
                                        break;
                                case 3:
                                        cprintf("Teleportation                   3 Magic, Food");
                                        break;
                                case 4:
                                        cprintf("Breathe Fire                    Food, Delay");
                                        break;
                                case 5:
                                        cprintf("Blink                           1 Magic, Food ");
                                        break;
                                case 6:
                                        cprintf("Breathe Frost                   Food, Delay");
                                        break;
                                case 7:
                                        cprintf("Breathe Poison Gas              Food, Delay");
                                        break;
                                case 8:
                                        cprintf("Breathe Lightning               Food, Delay");
                                        break;
                                case 9:
                                        cprintf("Spit Acid                       Food");
                                        break;
                                case 10:
                                        cprintf("Breathe Power                   Food, Delay");
                                        break;
                                case 11:
                                        cprintf("Go Berserk                      None");
                                        break;
                                case 12:
                                        cprintf("Breathe Sticky Flame            Food, Delay");
                                        break;
                                case 13:
                                        cprintf("Breathe Steam                   Food, Delay");
                                        break;
                                case 14:
                                        cprintf("Fly                             3 Magic, Food");
                                        break;
/* Demonic powers: */
                                case 15:
                                        cprintf("Summon Minor Demon              3 Magic, 3 hp, Food");
                                        break;
                                case 16:
                                        cprintf("Summon Demon                    5 Magic, 5 hp, Food");
                                        break;
                                case 17:
                                        cprintf("Hellfire                        8 Magic, 8 hp, Food");
                                        break;
                                case 18:
                                        cprintf("Torment                         9 Magic, Food");
                                        break;
                                case 19:
                                        cprintf("Raise Dead                      5 Magic, 5 hp, Food");
                                        break;
                                case 20:
                                        cprintf("Control Demon                   4 Magic, 4 hp, Food");
                                        break;
                                case 21:
                                        cprintf("Open Gate to Pandemonium        7 Magic, Food");
                                        break;
                                case 22:
                                        cprintf("Channeling                      1 hp, Food");
                                        break;
                                case 23:
                                        cprintf("Throw Flame                     1 Magic, 1 hp, Food");
                                        break;
                                case 24:
                                        cprintf("Throw Frost                     1 Magic, 1 hp, Food");
                                        break;
                                case 25:
                                        cprintf("Bolt of Draining                4 Magic, 4 hp, Food");
                                        break;
/* Most of these powers are from items: */
                                case 51:
                                        cprintf("Turn Invisible                  2 Magic, Food");
                                        break;
                                case 52:
                                        cprintf("Turn Visible                    None");
                                        break;
                                case 53:
                                        cprintf("Levitate                        1 Magic, Food");
                                        break;
                                case 54:
                                        cprintf("Stop Levitating                 None");
                                        break;
                                case 55:
                                        cprintf("End Transformation              None");
                                        break;
                                case 56:
                                        cprintf(" ");
                                        break;
/* Invocations: */
                                case 110:
                                case 120:
                                        cprintf("Repel Undead                    1 Magic, Food");
                                        break;
                                case 111:
                                        cprintf("Minor Healing                   2 Magic, Food, Piety");
                                        break;
                                case 112:
                                        cprintf("Pestilence                      3 Magic, Food, Piety");
                                        break;
                                case 113:
                                        cprintf("Holy Word                       6 Magic, Food, Piety");
                                        break;
                                case 114:
                                        cprintf("Summon Guardian                 7 Magic, Food, Piety");
                                        break;

                                case 121:
                                        cprintf("Smiting                         3 Magic, Food, Piety");
                                        break;
                                case 122:
                                        cprintf("Annihilate Undead               3 Magic, Food, Piety");
                                        break;
                                case 123:
                                        cprintf("Thunderbolt                     5 Magic, Food, Piety");
                                        break;
                                case 124:
                                        cprintf("Summon Daeva                    8 Magic, Food, Piety");
                                        break;

                                case 130:
                                        cprintf("Recall Undead Slaves            1 Magic");
                                        break;
                                case 132:
                                        cprintf("Enslave Undead                  2 Magic, Food, Piety");
                                        break;
                                case 133:
                                        cprintf("Invoke Death                    3 Magic, Food, Piety");
                                        break;

                                case 140:
                                        cprintf("Animate Corpse                  3 Magic, Food");
                                        break;
                                case 141:
                                        cprintf("Recall Undead                   4 Magic, Food");
                                        break;
                                case 142:
                                        cprintf("Animate Dead                    7 Magic, Food, Piety");
                                        break;
                                case 143:
                                        cprintf("Drain Life                      6 Magic, Food, Piety");
                                        break;
                                case 144:
                                        cprintf("Control Undead                  5 Magic, Food, Piety");
                                        break;

                                case 160:
                                        cprintf("Channel Energy                  Food");
                                        break;

                                case 170:
                                        cprintf("Might                           2 Magic, Food, Piety");
                                        break;
                                case 171:
                                        cprintf("Healing                         2 Magic, Food, Piety");
                                        break;
                                case 172:
                                        cprintf("Haste                           5 Magic, Food, Piety");
                                        break;

                                case 180:
                                        cprintf("Minor Destruction               1 Magic, Food");
                                        break;
                                case 181:
                                        cprintf("Lesser Servant of Makhleb       2 Magic, Food, Piety");
                                        break;
                                case 182:
                                        cprintf("Major Destruction               3 Magic, Food, Piety");
                                        break;
                                case 183:
                                        cprintf("Greater Servant of Makhleb      4 Magic, Food, Piety");
                                        break;


                                case 200:
                                        cprintf("Go Berserk                      Food");
                                        break;
                                case 201:
                                        cprintf("Might                           Food, Piety");
                                        break;
                                case 202:
                                        cprintf("Haste Self                      Food, Piety");
                                        break;

                                case 190:
                                        cprintf("Forget Spell                    5 Magic, Piety");
                                        break;

                                case 220:
                                        cprintf("Lesser Healing                  1 Magic, Food");
                                        break;
                                case 221:
                                        cprintf("Purification                    2 Magic, Food, Piety");
                                        break;
                                case 222:
                                        cprintf("Healing                         2 Magic, Food, Piety");
                                        break;
                                case 223:
                                        cprintf("Restoration                     3 Magic, Food, Piety");
                                        break;
                                case 224:
                                        cprintf("Greater Healing                 6 Magic, Food, Piety");
                                        break;

                                case 250:
                                        cprintf("Renounce Religion               None");
                                        break;

                        }

                        gotoxy(35, wherey());
                        already = 0;

                        gotoxy(60, wherey());

                        int spell_f = ability_fail [j];

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

                }
        } // end of j loop
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
                if (ki == 0) ki = getch();
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

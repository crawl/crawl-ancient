#include "config.h"

#ifdef USE_EMX
#include <sys/types.h>
#endif

#include <string.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <stdlib.h>
#include <unistd.h>

#ifdef DOS
#include <conio.h>
#endif

#include "externs.h"
#include "player.h"
#include "religion.h"
#include "itemname.h"
#include "macro.h"
#include "message.h"
#include "mutation.h"
#include "skills.h"
#include "spells0.h"
#include "shopping.h"


/*
Creates a disk record of a character.
*/
char dump_char(char show_prices, char fname [30])
{
        char st_prn [15];

        char dmp [200] [120]; // second is length of line

        char lin = 0;

        int i = 0;
        int j = 0;

        char file_name [20];
        char title [40];


        for (i = 0; i < 200; i ++)
        {
                strcpy(dmp [i], "");
        }

        strcpy(dmp [lin], you[0].your_name);
        strcat(dmp [lin], " the ");
        strcpy(title, skill_title(best_skill(0, 50, 99), you[0].skills [best_skill(0, 50, 99)], you[0].clas, you[0].xl));
        strcat(dmp [lin], title);
        strcat(dmp [lin], " (");
        strcat(dmp [lin], species_name(you[0].species));
        strcat(dmp [lin], ")");
//strcat(dmp [lin], ".");
        lin ++;

        strcpy(dmp [lin], "(Level ");
        itoa(you[0].xl, st_prn, 10);
        strcat(dmp [lin], st_prn);
        strcat(dmp [lin], " ");
        strcat(dmp [lin], you[0].clasnam);
        strcat(dmp [lin], ")");
        lin ++;

        strcpy(dmp [lin], "Experience : ");
        itoa(you[0].xl, st_prn, 10);
        strcat(dmp [lin], st_prn);
        strcat(dmp [lin], "/");
        itoa(you[0].xp, st_prn, 10);
        strcat(dmp [lin], st_prn);
        lin ++;
        strcpy(dmp [lin], "Strength ");
        itoa(you[0].strength, st_prn, 10);
        strcat(dmp [lin], st_prn);
        if (you[0].strength < you[0].max_strength)
        {
                strcat(dmp [lin], "/");
                itoa(you[0].max_strength, st_prn, 10);
                strcat(dmp [lin], st_prn);
        }
        strcat(dmp [lin], "     Dexterity ");
        itoa(you[0].dex, st_prn, 10);
        strcat(dmp [lin], st_prn);
        if (you[0].dex < you[0].max_dex)
        {
                strcat(dmp [lin], "/");
                itoa(you[0].max_dex, st_prn, 10);
                strcat(dmp [lin], st_prn);
        }
        strcat(dmp [lin], "     Intelligence ");
        itoa(you[0].intel, st_prn, 10);
        strcat(dmp [lin], st_prn);
        if (you[0].intel < you[0].max_intel)
        {
                strcat(dmp [lin], "/");
                itoa(you[0].max_intel, st_prn, 10);
                strcat(dmp [lin], st_prn);
        }
        lin ++;
        strcpy(dmp [lin], "Hit Points : ");
        itoa(you[0].hp, st_prn, 10);
        strcat(dmp [lin], st_prn);
        if (you[0].hp < you[0].hp_max)
        {
                strcat(dmp [lin], "/");
                itoa(you[0].hp_max, st_prn, 10);
                strcat(dmp [lin], st_prn);
                if (you[0].hp <= 0)
                {
                        if (you[0].deaths_door == 0)
                        {
                                strcat(dmp [lin], " ");
                                strcat(dmp [lin], "(dead)");
                        } else
                        {
                                strcat(dmp [lin], " ");
                                strcat(dmp [lin], "(almost dead)");
                        }
                }

        }
        strcat(dmp [lin], "          Magic Points : ");
        itoa(you[0].ep, st_prn, 10);
        strcat(dmp [lin], st_prn);
        if (you[0].ep < you[0].ep_max)
        {
                strcat(dmp [lin], "/");
                itoa(you[0].ep_max, st_prn, 10);
                strcat(dmp [lin], st_prn);
        }
        lin ++;
        strcpy(dmp [lin], "AC : ");
        itoa(player_AC(), st_prn, 10);
        strcat(dmp [lin], st_prn);
        strcat(dmp [lin], "          Evasion : ");
        itoa(player_evasion(), st_prn, 10);
        strcat(dmp [lin], st_prn);
        strcat(dmp [lin], "          Shield : ");
        itoa(player_shield_class(), st_prn, 10);
        strcat(dmp [lin], st_prn);
        lin ++;
        strcpy(dmp [lin], "Gp : ");
        itoa(you[0].gp, st_prn, 10);
        strcat(dmp [lin], st_prn);
        lin ++;
        strcpy(dmp [lin], "");
        lin ++;

        strcpy(dmp [lin], "You are ");

        if (you[0].level_type == 3)
        {
                strcat(dmp [lin], "in Pandemonium");
        }
        else if (you[0].level_type == 2)
        {
                strcat(dmp [lin], "in the Abyss");
        }
        else if (you[0].level_type == 1)
        {
                strcat(dmp [lin], "in a labyrinth");
        }
        else if (you[0].where_are_you == 1)
        {
                strcat(dmp [lin], "in Dis");
        }
        else if (you[0].where_are_you == 2)
        {
                strcat(dmp [lin], "in Gehenna");
        }
        else if (you[0].where_are_you == 3)
        {
                strcat(dmp [lin], "in the Vestibule of Hell");
        }
        else if (you[0].where_are_you == 4)
        {
                strcat(dmp [lin], "in Cocytus");
        }
        else if (you[0].where_are_you == 5)
        {
                strcat(dmp [lin], "in Tartarus");
        }
        else if (you[0].where_are_you == 6)
        {
                strcat(dmp [lin], "in the Inferno");
        }
        else if (you[0].where_are_you == 7)
        {
                strcat(dmp [lin], "in the Pit");
        }
        else
        {
                if (you[0].your_level == -1)
                {
                strcpy(dmp [lin], "You escaped.");
                } else
                {
                    strcat(dmp [lin], "on level ");
                    itoa(you[0].your_level + 1, st_prn, 10);
                    strcat(dmp [lin], st_prn);
                }
        }
        lin ++;

        if (you[0].religion != 0)
        {
                strcpy(dmp [lin], "You worship ");
                strcat(dmp [lin], god_name(you[0].religion));
                strcat(dmp [lin], ".");
                lin ++;
                strcpy(dmp [lin], god_name(you[0].religion));

                if (you[0].religion != 5) /* Xom doesn't care */
                {
                        if (you[0].piety <= 5)
                        {
                                strcat(dmp [lin], " is displeased.");
                        } else
                        if (you[0].piety <= 20)
                        {
                                strcat(dmp [lin], " is noncommittal.");
                        } else
                        if (you[0].piety <= 40)
                        {
                                strcat(dmp [lin], " is pleased with you.");
                        } else
                        if (you[0].piety <= 70)
                        {
                                strcat(dmp [lin], " is most pleased with you.");
                        } else
                        if (you[0].piety <= 100)
                        {
                                strcat(dmp [lin], " is greatly pleased with you.");
                        } else
                        if (you[0].piety <= 130)
                        {
                                strcat(dmp [lin], " is extremely pleased with you.");
                        } else
                        {
                                strcat(dmp [lin], " is exalted by your worship.");
                        }
                        lin ++;
                }
        }

        switch(you[0].burden_state)
        {
                case 5: strcpy(dmp [lin], "You are overloaded with stuff."); lin ++; break;
                case 2: strcpy(dmp [lin], "You are encumbered."); lin ++; break;
        }

        if (you[0].hunger <= 1000)
        {
                strcpy(dmp [lin], "You are starving.");
        } else if (you[0].hunger <= 2600)
        {
                strcpy(dmp [lin], "You are hungry.");
        } else if (you[0].hunger < 7000)
        {
                strcpy(dmp [lin], "You are not hungry.");
        } else if (you[0].hunger < 11000)
        {
                strcpy(dmp [lin], "You are full.");
        } else
                strcpy(dmp [lin], "You are completely stuffed.");


        lin += 2;


        switch(you[0].attribute [5])
        {
                case 1: strcpy(dmp [lin], "You are in spider-form."); lin += 2; break;
                case 2: strcpy(dmp [lin], "Your hands are blades."); lin += 2; break;
                case 3: strcpy(dmp [lin], "You are a stone statue."); lin += 2; break;
        }




        char st_pass [60];

    char lines = 0;
    unsigned char anything = 0;
    char ft;
    char yps = 0;
    char temp_id [4] [50];

    for (i = 0; i < 4; i ++)
        {
        for (j = 0; j < 50; j ++)
            {
                temp_id [i] [j] = 1;
        }
        }




        strcpy(st_pass, "");



        int Inv_class2 [15];
        int inv_count = 0;
        char strng [80];

        for     (i = 0; i < 15; i++)
        {
                Inv_class2 [i] = 0;
        }



        for (i = 0; i < 52; i++)
        {
                if (you[0].inv_quant [i] != 0)
                {
                        Inv_class2 [you[0].inv_class [i]] ++; // adds up number of each class in invent.
                        inv_count++;
                }
        }

        if (inv_count == 0)
        {
                strcpy(dmp [lin], "You aren't carrying anything.");
                lin ++;
                goto finished_invent;
        }


//if ((item_class_inv == -1 && you[0].inv_count > 0) || (item_class_inv != -1 && Inv_class2 [item_class_inv] > 0) || (item_class_inv == 1 && (Inv_class2 [0] > 0 || Inv_class2 [1] > 0)) || )// || (item_class_inv == 3 && (Inv_class2 [3] > 0 || Inv_class2 [11] > 0)))
//{

        strcpy(dmp [lin], "  Inventory:");
        lin ++;

        for (i = 0; i < 15; i ++)
        {
                if (Inv_class2 [i] != 0)
                {
                        if (lines > 0) lin ++;
                        switch(i)
                        {
                                case 0: strcpy(dmp [lin], "Hand weapons"); break;
                                case 1: strcpy(dmp [lin], "Missiles"); break;
                                case 2: strcpy(dmp [lin], "Armour"); break;
                                case 3: strcpy(dmp [lin], "Magical devices"); break;
                                case 4: strcpy(dmp [lin], "Comestibles"); break;
                        case 5: strcpy(dmp [lin], "Books"); break;
                                case 6: strcpy(dmp [lin], "Scrolls"); break;
                                case 7: strcpy(dmp [lin], "Jewellery"); break;
                                case 8: strcpy(dmp [lin], "Potions"); break;
                                case 9: strcpy(dmp [lin], "Gems"); break;
                                case 10: strcpy(dmp [lin], "Books"); break;
                                case 11: strcpy(dmp [lin], "Magical staves"); break;
                                case 12: strcpy(dmp [lin], "Orbs of Power"); break;
                                case 13: strcpy(dmp [lin], "Miscellaneous"); break;
                                case 14: strcpy(dmp [lin], "Carrion"); break;
                        }

                        lin++;

                        for (j = 0; j < 52; j++)
                        {

                                if (you[0].inv_class [j] == i && you[0].inv_quant [j] > 0)
                                {

                                        anything ++;
        //                              if (lines > 0) cprintf("\n\r");
//                      lines++;
                                        strcpy(dmp [lin], " ");
                                        if (j < 26) ft = (char) j + 97;
                                        else ft = (char) j  + 65 - 26;

                                        strng [0] = ft;
                        strng [1] = 0;
                                        strcat(dmp [lin], strng);
                                        strcat(dmp [lin], " - ");

                                        yps = wherey();

                                        item_name(you[0].inv_plus2 [j], you[0].inv_class [j], you[0].inv_type [j], you[0].inv_dam [j], you[0].inv_plus [j], you[0].inv_quant [j], you[0].inv_ident [j], 3, st_pass);
                                        strcat(dmp [lin], st_pass);

                                    inv_count --;



                                        if (j == you[0].equip [0]) strcat(dmp [lin], " (weapon)");
                                        if (j == you[0].equip [6] || j == you[0].equip [1] || j == you[0].equip [2] || j == you[0].equip [3] || j == you[0].equip [4] || j == you[0].equip [5]) strcat(dmp [lin], " (worn)");
                                        if (j == you[0].equip [7]) strcat(dmp [lin], " (left hand)");
                                        if (j == you[0].equip [8]) strcat(dmp [lin], " (right hand)");
                                        if (j == you[0].equip [9]) strcat(dmp [lin], " (neck)");

                                        if (show_prices == 1)
                                        {
                                                strcat(dmp [lin], " (");
                                                itoa(item_value(you[0].inv_class [j], you[0].inv_type [j], you[0].inv_dam [j], you[0].inv_plus [j], you[0].inv_plus2 [j], you[0].inv_quant [j], 3, temp_id), strng, 10);
                                                strcat(dmp [lin], strng);
                                                strcat(dmp [lin], "gp)");
                                        }

                                        lin ++;

                                }
                        } // end of j loop
                } // end of if inv_class2
        } // end of i loop.

        finished_invent:

        lin += 2;
        strcpy(dmp [lin], " You have ");
        itoa(you[0].exp_available, strng, 10);
        strcat(dmp [lin], strng);
        strcat(dmp [lin], " experience left.");


        lin += 2;
        strcpy(dmp [lin], "   Skills:");
        lin += 1;

        for (i = 0; i < 50; i ++)
        {
                if (you[0].skills [i] > 0)
                {
                        if (you[0].practise_skill [i] == 0) strcpy(dmp [lin], " - "); else strcpy(dmp [lin], " + ");

                        strcat(dmp [lin], "Level ");
                        itoa(you[0].skills [i], strng, 10);
                        strcat(dmp [lin], strng);
                        strcat(dmp [lin], " ");
                        strcat(dmp [lin], skill_name(i));
                        lin++;
                }
        }

        lin += 2;


        if (you[0].spell_levels == 1)
        {
                strcpy(dmp [lin], "You have one spell level left.");
        } else if (you[0].spell_levels == 0)
        {
                strcpy(dmp [lin], "You cannot memorise any spells.");
        } else
        {
                strcpy(dmp [lin], "You have ");
                itoa(you[0].spell_levels, strng, 10);
                strcat(dmp [lin], strng);
                strcat(dmp [lin], " spell levels left.");
        }

        lin += 1;


        if (you[0].spell_no == 0)
        {
                strcpy(dmp [lin], "You don't know any spells.");
                lin ++;
                goto finished_spells;
        }

        strcpy(dmp [lin], "You know the following spells:");
        lin++;

        for (j = 0; j < 25; j ++)
        {
                if (you[0].spells [j] != 210)
                {

                        strcpy(dmp [lin], " ");
                        if (j < 26) ft = (char) j + 97;
                        else ft = (char) j  + 65;
                        strng [0] = ft;
                        strng [1] = 0;
                        strcat(dmp [lin], strng);
                        strcat(dmp [lin], " - ");
                        spell_name(you[0].spells [j], st_pass);

                        strcat(dmp [lin], st_pass);
                        lin ++;

        }
        }

finished_spells:


        if (how_mutated() > 0)
        {

                strcpy(dmp [lin], "");
                lin++;
                strcpy(dmp [lin], "           Mutations & Other Weirdness");
                lin ++;

                for (j = 0; j < 50; j ++)
                {
                        if (you[0].mutation [j] == 0) continue;
                        strcpy(dmp [lin], mutation_name(j));
                        lin ++;
                }

        }


        for (i = 0; i < lin; i ++)
        strcat(dmp [i], EOL);
        /*#ifdef PLAIN_TERM
  strcat(dmp [i], "\n");
#endif

#ifdef DOS_TERM
 strcat(dmp [i], "\n");
#endif*/

        strcpy(file_name, fname);
        if (strcmp(file_name, "morgue.txt") != 0) strcat(file_name, ".txt");

        int handle = open(file_name, O_RDWR | O_CREAT | O_TRUNC | O_BINARY, 0660);

        strcpy(info, "File name: ");
        strcat(info, file_name);
        mpr(info);

        if (handle == -1)
        {
                mpr("Error opening file.");
                return -1;
        }

        int str_length = 0;

        for (i = 0; i < lin; i ++)
        {
                str_length = strlen(dmp [i]);
                if (str_length > 79) str_length = 79;
                write(handle, dmp [i], str_length);
        }

        close(handle);

        return 1;

}

#include "crawlfnc.h"
#include "fn2.h"
#include "monsstat.h"
#include "fn3.h"
#include "fn4.h"
#include "builder2.h"
#include "invent.h"

/*#include "\borlandc\crawl\crawlfnc.h"
#include "\borlandc\crawl\fn2.h"
#include "\borlandc\crawl\monsstat.h"
#include "\borlandc\crawl\fn3.h"
#include "fn4.h"*/

//void mons_place(void);

int mons_place(int typed, int type_place, int px, int py, char behaviour, int hitting);

//void Wcheck(void);
void monster_attack(int monster_attacking);
char monsters_fight(int monster_attacking, int monster_attacked);
void you_attack(int monster_attacked);
void monster_die(int monster_killed);
void level_change(void);
void lose_level(void);

char *monam(unsigned char mons_sc, int mons_cla, char mons_e, char desc);

char mons_near(unsigned char monst);

char empty_surrounds(int emx, int emy);

void incr(void);
void mesclr(void);
void relay_message(void);

void ouch(int dam, char death_source, char death_type);

int which_cloud(char cx, char cy);
void in_a_cloud(void);

void end_game(char end_status);

void splash_with_acid(char acid_strength);

void weapon_acid(char acid_strength);

void item_corrode(char itco);

char see_grid(unsigned char grx, unsigned char gry);

void monster_polymorph(unsigned char mons, unsigned char targetc, int power);

void monster_drop_ething(int monster_killed);

int jelly_divide(int jel);

void alert(void);

int mons_ench_f2(int o, char is_near, int func_pass [10]);

void poison_monster(int mn, char source); // 0 is you, 1 is something else

char curse_an_item(char which, char power);

void monster_blink(int mn);
char random_near_space(int passed [2]);

void unwield_item(char unw);
void unwear_armour(char unw);

char create_monster(int cls, int dur, int beha, int cr_x, int cr_y);

void swap_places(int swap);
int distance(int x1, int x2, int y1, int y2);

//void place_cloud(void);
void place_cloud(unsigned char cl_type, unsigned char ctarget_x, unsigned char ctarget_y, unsigned char cl_range);

void scrolls_burn(char burn_strength, char target_class);

void alert2(void);

char detect_traps(void);
char you_resist_magic(int power);
void forget_map(void);
void how_hungered(int hunge);


/*
// Character attributes:
int hp = 10; int hp_ch = 1;
int hp_max = 10; int hp_max_ch = 1;
int ep = 3; int ep_ch = 1;
int ep_max = 5; int ep_max_ch = 1;
unsigned int strength = 16; int strength_ch = 1;
unsigned int intel = 13; int intel_ch = 1;
unsigned int dex = 11; int dex_ch = 1;
int AC = 0; int AC_ch = 1; // remember that the AC shown = 10 - AC
int evasion = 10; int evasion_ch = 1;
int damage = 3; int damage_ch = 1;  // remember to change damage back to 1!
int nat_damage = 3; // the damage you do unarmed
float rate_regen = 0.06; float incr_regen = 0;
int xp = 0; int xp_ch = 1;
int xl = 1; int xl_ch = 1;
int gp = 0; int gp_ch = 1;
int clas;
*/

int ogo = 0;

void get_monster(void)
{

define_monster(mons_class, mons_hp, mons_hp_max, mons_HD, mons_AC, mons_ev,
 mons_speed, mons_speed_inc, mons_sec, k);

/*
mons_HD [k] = mons_define(mons_class [k], 0, mons_sec [k]);
mons_hp [k] = mons_define(mons_class [k], 1, mons_sec [k]);
//mons_hp_max [k] = mons_define(mons_class [k], 2); // can be different initially
mons_hp_max [k] = mons_hp [k];
mons_AC [k] = mons_define(mons_class [k], 3, mons_sec [k]);
mons_ev [k] = mons_define(mons_class [k], 4, mons_sec [k]);
mons_speed [k] = mons_define(mons_class [k], 5, mons_sec [k]);
mons_speed_inc [k] = mons_define(mons_class [k], 6, mons_sec [k]);
mons_sec [k] = mons_define(mons_class [k], 7, mons_sec [k]);
//mons_HD [k] = mons_define(mons_class [k], 0);
*/
}



int mons_place(int typed, int type_place, int px, int py, char behaviour, int hitting)
{
int passed [2];
passed [0] = 0; // DO t worms!!!!!!!!!!!!!!!! plus mention numbered dropping/getting in docs
passed [1] = 0;

char bands = 1;

if (char_direction == 1 && typed == 250)
{
 typed = 80 + random2(6);
 if (random2(5) == 0) typed = 3;
 if (random2(5) == 0) typed = 73;
}

if (typed != 250) bands = 0;

if (no_mons == MNST - 1) return 0;

int glokp = place_monster(mons_class,
mons_hp,
mons_hp_max,
mons_HD,
mons_AC,
mons_ev,
mons_speed,
mons_speed_inc,
mons_inv,
mons_sec,
mons_ench_1,
mons_ench,
mons_hit,
mons_beh,
it_no,
item_ident,
item_class,
unique_items,
item_type,
item_plus,
item_dam,
item_quant,
item_x,
item_y,
icolour,
1, // xtra_quant
item_link,
item_description,
property,
mons_x,
mons_y,
0, //plus_seventy,
typed, //250, // typed,
type_place, //0, //type_place,
px, //1, //px,
py, //1, //py,
behaviour, //0, //behaviour,
hitting, //MHITNOT, //hitting,
bands,//1,//1,//allow_bands
gmon_use,
grid,
mons_alloc,
passed,
igrid,
x_pos,
y_pos
);

no_mons += glokp;
mn_no += glokp;

if (glokp > 0) return passed [0];
                else return -1;

//return glokp;



















int passout = 0;

int worm = 0;
int pets = 0;
int grid_ok = 17;

//m_all(mons_alloc, curr_level);


// maybe put mons_alloc tables into the level save file so they can be
// tailored for specific levels.

k = 0;



do
{
        k ++;
} while (mons_class [k] != -1 && k < 182);

if (k >= 180) return -1;

//return -1;







// Eliminate that nasty pointer problem by passing a string to those
// functions and getting them to fill it in without returning anything!








if (typed == 250)
{

monster_cla : mons_class [k] = mons_alloc [random2(20)];

if (type_place == 1 && mons_class [k] == 19) goto monster_cla;
// can't summon t; shouldn't summon d, F, i?, q? or &
//mons_sec [k] = 250;
mons_class [k] = random2(67);
//if (random2(2) == 0) mons_class [k] = 54;//MLAVA0 + random2(3);
//mons_class [k] = 47;// + random2(8);

} else mons_class [k] = typed;



if (mons_class [k] >= MLAVA0)
{
 mons_sec [k] = 0;
 if (mons_class [k] >= MWATER0) grid_ok = 12; else grid_ok = 11;
}
//mons_sec [k] = random2(40);

worm_tail:
if (worm > 0 && worm < 5)
{
        k++;
        if (mons_class [k] != -1)
        {
                killer = 5;
                monster_die(k);
                strcpy(info [info_lines], "Ugg!");
                incr();
        }
        mons_class [k] = 56;
        worm++;
}

if (mons_class [k] == 19)
{
        //if (mons_class [k + 1] == -1 &&
        /*
        mons_class [k] = -1;

        char d = 0;
        char bkl = 0;

        do
        {
        char g = 0;

        for (g = k; g < k + 5; k ++)
        {
                if (mons_class [g] != -1)
                {
                        d = 0;
                        k++;
                        cout << (int) k << ", "; getch();
                        bkl = 1;
                        goto b_out;
                }
        }

        b_out: if (bkl == 1)
                {
                        bkl = 0;
                        continue;
                }

        d = 1;
        mons_class [k] = 19;

        } while (d == 0);
        /*
        for (g = k; g < k + 5; k ++)
        {
                if (mons_class [g] != -1) goto monster_cla;
        }
        */
worm = 1;
}














//                      mons_class [k] = 8;

mons_sec [k] = 250;

get_monster();



/*
mons_HD [k] -= 100;
mons_hp [k] -= 100;
mons_hp_max [k] -= 100;
mons_AC [k] -= 100;
mons_ev [k] -= 100;
mons_speed [k] -= 100;
mons_speed [k] /= 10;
mons_speed_inc [k] -= 100;
*/

//mons_speed [k] /= 10;


no_mons ++;
mn_no ++;

mons_beh [k] = behaviour; // Look at this
mons_hit [k] = hitting; //MHITNOT;
/*if (behaviour == 7)
{
 for (pets = 0; pets < PETS; pets ++)
 {
        if (pet_which [pets] == 99)
        {
   pet_which [pets] = k;
                        break;
        }
 } // end of for pets
} // end pets*/

if (mons_class [k] == 46)
{
 mons_ench_1 [k] = 1;
 mons_ench [k] [2] = 6;
}

if (mons_class [k] == 66)
{
        mons_ench_1 [k] = 1;
        mons_ench [k] [0] = 5;
}

if (type_place == 1) // summoned
{
   mons_x [k] = px;
   mons_y [k] = py;
}
else
 {
 passout = 0;
 while (passout < 300)
 {
 mons_x [k] = 10;// + random2(120) + 100;
 mons_y [k] = 10 + random2(60);// + 100;
 mons_x [k] += random2 (70);
 //mons_x [k] += 100;
 passout ++;
 if (grid [mons_x [k]] [mons_y [k]] == grid_ok && (mons_x [k] != x_pos | mons_y [k] != y_pos)) break; //passout = 1;
 }

 if (passout == 300)
 {
        mons_class [k] = -1;
                return -1;
 }

 if (mons_class [k] == 56)
 {
        mons_x [k] = mons_x [k - 1] + 1;
        mons_y [k] = mons_y [k - 1];
 }

} // end of else

grid [mons_x [k]] [mons_y [k]] += 50;















                if (worm > 0 && worm < 5) goto worm_tail;
















/*
strcpy(info [info_lines], "Giving: ");
itoa(it_no, st_prn, 10);
strcat(info [info_lines], st_prn);
*/

it_no += restock_monsters(mons_class, mons_inv, item_class, item_type, item_dam, icolour, item_plus, item_quant, your_level, grid, item_x, item_y, it_no, k, mons_x, mons_y, item_link, item_ident);

/*
strcat(info [info_lines], ", ");
itoa(it_no, st_prn, 10);
strcat(info [info_lines], st_prn);
incr();
*/

if (mons_sec [k] == 250) mons_sec [k] = 0;

return k;


} // end of mons_place()






/*
void Wcheck(void)
{
brek = 0;
for (count_x = 0; count_x < 80; count_x ++)
{
        for (count_y = 0; count_y < 70; count_y ++)
        {
                if (grid [count_x] [count_y] == 87)
                {
                for (i = 0; i < MNST; i++)
                {
                        if (mons_x [i] == count_x && mons_y [i] == count_y && mons_class [i] != -1)
                                brek = 1;

                }
//              if (brek == 0) cout << "Problem with monster " << i << "at " << count_x << "," << count_y << "\n";
                brek = 0;
                //cout << "Prboblsdflkgjksdfjglksdg";
                }
        }
}

} // end of void Wcheck()
*/




void monster_attack(int monster_attacking)
{
//float mons_to_hit;
damage_taken = 0;
char blocked = 0;
char hit = 0;

int specdam = 0;

char heads = 0;
if (mons_class [monster_attacking] == 106) heads = mons_sec [monster_attacking];

hand_used = 0;

if (mons_class [monster_attacking] == 32)
{
                mons_hp [monster_attacking] = -1;
                return;
}

//if (pet_target == MHITNOT)
{
 pet_target = monster_attacking;
}

//int runthru = 0;

//mons_to_hit = mons_HD [monster_attacking]; // not finished yet.

if (mons_beh [monster_attacking] == 7) return;

if (mons_class [monster_attacking] >= MLAVA0 && mons_sec [monster_attacking] == 1) return;

for (runthru = 0; runthru < 4; runthru ++)
{

if (mons_class [monster_attacking] == 106)
{
 if (heads <= 0) break;
 runthru = 0;
 heads --;
}

char mdam = mondamage(mons_class [monster_attacking], runthru);

if (mons_class [monster_attacking] == 25 | mons_class [monster_attacking] == 51)
{
        mdam = mondamage(mons_sec [monster_attacking], runthru);
        if (mdam > 1 && mdam < 4)
        {
                mdam --;
        }

        if (mdam > 3 && mdam < 8)
        {
                mdam -= 2;
        }

        if (mdam > 7 && mdam < 13)
        {
                mdam -= 3;
        }

        if (mdam > 12)
        {
                mdam -= 5;
        }
}


if (mdam == 0) break;
//gmon_att [mons_class [monster_attacking]] [runthru] == 0) break;

if (mons_class [monster_attacking] == 30 && runthru == 1 && mons_inv [monster_attacking] [1] != 501)
{
        hand_used = 1;
}

damage_taken = 0;


mons_to_hit = 20 + mons_HD [monster_attacking]; // * mons_HD [monster_attacking]; // * 3  //strength + (0.5 * dex) + (f_abil);

if (mons_inv [monster_attacking] [hand_used] != 501)
{
        if (item_plus [mons_inv [monster_attacking] [hand_used]] - 50 > 130)
        {
                mons_to_hit += random2(item_plus [mons_inv [monster_attacking] [hand_used]] - 50 - 100 + 1);
        } else
              {
                mons_to_hit += random2(item_plus [mons_inv [monster_attacking] [hand_used]] - 50 + 1); /// 10;
              }

//      mons_to_hit += item_plus [mons_inv [monster_attacking] [0]];
        mons_to_hit += 3 * property [0] [item_type [mons_inv [monster_attacking] [0]]] [1];

   if (mons_speed_inc [monster_attacking] >= 25) mons_speed_inc [monster_attacking] -= (float) ((float) (property [item_class [mons_inv [monster_attacking] [0]]] [item_type [mons_inv [monster_attacking] [0]]] [2] - 10)); // / 10);
                                                                                                                                                                                                                                                                                                // a good safety margin
}

//if (inv_class [item_wielded] == 0) mons_to_hit += inv_plus [item_wielded];


//mons_to_hit *= random2(200) / 100;

mons_to_hit = random2(mons_to_hit);

/*
//if (mons_to_hit >= (evasion - 6)) //&& mons_to_hit < evasion && shield_armour != -1) // && random2(6) != 0)
if (mons_to_hit < evasion)
{
strcpy(info [info_lines], "You block the ");
strcat(info [info_lines], gmon_name [mons_class [monster_attacking]]);
if (mons_inv [monster_attacking] [0] != 255)
        {
        strcat(info [info_lines], "'s ");
        strcat(info [info_lines], fake_name [mmov_x]);
        }
strcat(info [info_lines], " on your shield!");
incr();
return;
}
*/


        // This bit is a bit clumsy:
   //      not any more.
        if (mons_class [monster_attacking] == 10 | mons_class [monster_attacking] == 22)
        {
/*              if (haste == 0) mons_speed_inc [monster_attacking] -= (mons_speed [monster_attacking] * time_taken);
                        else mons_speed_inc [monster_attacking] -= (mons_speed [monster_attacking] * (time_taken / 2));

                if (slow > 0) mons_speed_inc [monster_attacking] -= (mons_speed [monster_attacking] * time_taken);*/

                // maybe this will work better:
  if (mons_speed_inc [monster_attacking] >= 15)
                                                mons_speed_inc [monster_attacking] -= 10; //--
        }


if (shield_class > 0 && paralysis == 0 && conf == 0 && random2(mons_HD [monster_attacking] + 10) <= random2(shield_class))
{
 strcpy(info [info_lines], "You block ");
        strcat(info [info_lines], monam (mons_sec [monster_attacking], mons_class [monster_attacking], mons_ench [monster_attacking] [2], 1));  //gmon_name [mons_class [monster_attacking]]);
 strcat(info [info_lines], "'s attack.");
 incr();
 blocked = 1;
 hit = 0;
 //continue;
} else
if (mons_to_hit >= evasion + random2(dex) / 3 - 2)// | random2(6) == 0) // mons_ev [monster_attacked])
//if (random2(20) + mons_to_hit >= 20 - (10 - AC)) // hit roll
{
hit = 1;

//te = mons_inv [monster_attacking] [0];

//k = item_class [te];
//p = item_type [te];
//j = property [k] [p] [0];
//o = mons_class [monster_attacking];
//k = gmon_att [o] [runthru];


if (mons_inv [monster_attacking] [hand_used] != 501 && item_class [mons_inv [monster_attacking] [hand_used]] == 0 && item_type [mons_inv [monster_attacking] [hand_used]] < 13 | item_type [mons_inv [monster_attacking] [hand_used]] > 15)
{
//damage_taken = random2(mmov_x);

damage_taken = random2(property [item_class [mons_inv [monster_attacking] [0]]] [item_type [mons_inv [monster_attacking] [0]]] [0]);
//if (item_plus [mons_inv [i] [0]] > 80) damage_taken -= 100;
//damage_taken += item_plus [mons_inv [i] [0]];
        if (item_plus [mons_inv [monster_attacking] [hand_used]] > 130)
        {
                damage_taken += random2(item_plus [mons_inv [monster_attacking] [hand_used]] - 150 + 1);
        } else
              {
                damage_taken += random2(item_plus [mons_inv [monster_attacking] [hand_used]] - 50 + 1);// % 10;
              }
damage_taken -= random2(3) + 1;//1;
}

damage_taken += random2(mdam) + 1;
//gmon_att [mons_class [monster_attacking]] [runthru]) + 1;
//damage_taken /= (random2 (AC) + 1); // / 3
//damage_taken *= 2;

//if (mons_inv [monster_attacking] [0] != 501)
//{
//      damage_taken /= ((random2 (AC) / property [item_class [mons_inv [i] [0]]] [item_type [mons_inv [i] [0]]] [2]) + 1);
//} else
//      damage_taken /= (random2 (AC) + 1);

        damage_taken -= random2(AC + 1);

if (damage_taken < 1) damage_taken = 0;
//hp -= (int) damage_taken;
//ouch(damage_taken, monster_attacking, 0);

//hp_ch = 1;

} // end of if mons_to_hit >= evasion
 else
 {
        hit = 0;
//      strcpy(info [info_lines], "The ");
        //} else
        //strcat(info [info_lines], "The ");

        strcpy(info [info_lines], monam (mons_sec [monster_attacking], mons_class [monster_attacking], mons_ench [monster_attacking] [2], 0));  //gmon_name [mons_class [monster_attacking]]);
        strcat(info [info_lines], " misses you.");
        incr();
 }


if (damage_taken < 1 && hit == 1)
{
//if (78 - strlen(info [info_lines]) < strlen(gmon_name [mons_class [monster_attacking]]) + 17)
//      {
        //incr();
//      strcpy(info [info_lines], "The ");
        //} else
        //strcat(info [info_lines], "The ");


        strcpy(info [info_lines], monam (mons_sec [monster_attacking], mons_class [monster_attacking], mons_ench [monster_attacking] [2], 0));  //gmon_name [mons_class [monster_attacking]]);
        strcat(info [info_lines], " hits you but doesn't do any damage.");
        incr();
}



if ((int) damage_taken >= 1)
{
//if (78 - strlen(info [info_lines]) < strlen(gmon_name [mons_class [monster_attacking]]) + 15)
//      {
//      incr();
//      strcpy(info [info_lines], "The ");
//      } else
//      strcpy(info [info_lines], "The ");

 hit = 1;
        mmov_x = mons_inv [monster_attacking] [hand_used];

        strcpy(info [info_lines], monam (mons_sec [monster_attacking], mons_class [monster_attacking], mons_ench [monster_attacking] [2], 0));
        strcat(info [info_lines], " hits you");



/*        strcat(info [info_lines], " for ");
        itoa(damage_taken, st_prn, 10);
        strcat(info [info_lines], st_prn);
*/


//      itoa(

        if (mons_inv [monster_attacking] [hand_used] != 501 && item_class [mons_inv [monster_attacking] [hand_used]] == 0 && item_type [mons_inv [monster_attacking] [hand_used]] < 13 | item_type [mons_inv [monster_attacking] [hand_used]] > 15)
        {
        strcat(info [info_lines], " with ");

        item_name(item_class [mmov_x], item_type [mmov_x], item_dam [mmov_x], item_plus [mmov_x], item_quant [mmov_x], item_ident [mmov_x], 7, str_pass);
        strcat(info [info_lines], str_pass);
 strcat(info [info_lines], "!");
 //incr();
 // special weapons:



 } else
   {
    strcat(info [info_lines], "!");
   }
        //strcat(info [info_lines], "! "); // put something for '.', '!', '!!' etc depending on what % of damage you took.

incr();



// special attacks:

        brek = 0;



        switch(mons_class [monster_attacking])
        {
        case 0: // giant ant
        if ((damage_taken >= 4 && random2(4) == 0) | random2(20) == 0)
        {
 strcpy(info [info_lines], monam (mons_sec [monster_attacking], mons_class [monster_attacking], mons_ench [monster_attacking] [2], 0));
        strcat(info [info_lines], " stings you!");
        incr();
        poison++;
        }
        break;

        case 2: // cockatrice.
        break;

        case 10: // killer bee
        if (res_poison == 0 && ((damage_taken >= 3 && random2(3) == 0) | random2(20) == 0))
        {
 strcpy(info [info_lines], monam (mons_sec [monster_attacking], mons_class [monster_attacking], mons_ench [monster_attacking] [2], 0));
        strcat(info [info_lines], " stings you!");
        incr();
        poison += 1;
        }
        break;

 case 81: // rotting devil
        case 13: // necrophage
                if ((damage_taken >= 3 && random2(3) == 0) | random2(20) == 0)
                {
                        strcpy(info [info_lines], "You feel your flesh start to rot away!");
                        incr();
                        rotting += random2(3) + 1;
                }
                break;


 case 21: // fire vortex
 mons_hp [monster_attacking] = -10;
        strcpy(info [info_lines], "You are engulfed in flame!");
//        strcpy(info [info_lines], monam (mons_sec [monster_attacking], mons_class [monster_attacking], mons_ench [monster_attacking] [2], 0));
//        strcat(info [info_lines], " burns you!");
        incr();
//        damage_taken += specdam;
   if (res_fire == 0)
   {
        damage_taken += 15 + random2(15);
   }
 scrolls_burn(1, 6);
 break;


        case 44: // scorpion
 case 100: // giant mite
        if (res_poison == 0 && ((damage_taken >= 3 && random2(4) == 0) | random2(15) == 0))
        {
 strcpy(info [info_lines], monam (mons_sec [monster_attacking], mons_class [monster_attacking], mons_ench [monster_attacking] [2], 0));
        strcat(info [info_lines], " poisons you!");
        incr();
        poison++;
        }
        break;



        case 42: // Queen bee
if (res_poison == 0)
{
 strcpy(info [info_lines], monam (mons_sec [monster_attacking], mons_class [monster_attacking], mons_ench [monster_attacking] [2], 0));
        strcat(info [info_lines], " stings you!");
        incr();
        poison += 2;
}
        break;


        case 18: // snake
        if (res_poison == 0 && ((damage_taken >= 3 && random2(4) == 0) | random2(20) == 0))
        {
 strcpy(info [info_lines], monam (mons_sec [monster_attacking], mons_class [monster_attacking], mons_ench [monster_attacking] [2], 0));
        strcat(info [info_lines], " stings you!");
        incr();
        poison++;
        }
        break;

 case 60: // Wight. Is less likely because wights do less damage
        case 48: // wraith
 case 84: // shadow devil
        if (((damage_taken >= 6 && random2(4) == 0) | random2(30) == 0) && prot_life == 0)
        {
        strcpy(info [info_lines], "You feel drained...");
        incr();
        brek = 1;
        }
        break;

        case 24: // yellow wasp
        if (res_poison == 0 && ((damage_taken >= 3 && random2(3) == 0) | random2(20) == 0))
        {
        if (paralysis > 0)
        {
                strcpy(info [info_lines], "You still can't move!");
        } else strcpy(info [info_lines], "You suddenly lose the ability to move!");
        incr();
        paralysis += random2(3) + 1;
        }
        break;

        case 35: // Jelly
        strcpy(info [info_lines], "You are splashed with acid!");
        incr();
        splash_with_acid(10);
        break;

 case 86: // ice devil
 case 34: // Ice beast
 case 77: // blue wraith
   if (res_cold == 0)
   {
        damage_taken += 5 + random2(10);
        strcpy(info [info_lines], monam (mons_sec [monster_attacking], mons_class [monster_attacking], mons_ench [monster_attacking] [2], 0));
        strcat(info [info_lines], " freezes you!");
        incr();
//        damage_taken += specdam;
   }
 scrolls_burn(1, 8);
 break;

 case 47: // Vampire
      if (damage_taken >= 7 && random2(3) == 0 | random2(20) == 0)
      {
        strcpy(info [info_lines], "You feel less resilient.");
        incr();
        hp_max -= random2(2) + 1;
        hp_ch = 1;
        if (hp >= hp_max) hp = hp_max;
      }
 break;

 case 63: // shadow
      if (((damage_taken >= 1 && random2(3)) == 0 | random2(20) == 0) && strength > 3 && sust_abil == 0)
      {
        strcpy(info [info_lines], "You feel weaker.");
        incr();
        strength --;
        strength_ch = 1;
      }
 break;

 case 64: // hungry ghost
      if (damage_taken >= 1 && random2(3) == 0 | random2(20) == 0)
      {
        strcpy(info [info_lines], "You feel hungry.");
        incr();
        hunger -= 400;
        //strength_ch = 1;
      }
 break;


        case 39: // Naga

        break;

 case 15: // phantom
 if (random2(3) == 0)
 {
   strcpy(info [info_lines], monam (mons_sec [monster_attacking], mons_class [monster_attacking], mons_ench [monster_attacking] [2], 0));
   strcat(info [info_lines], " blinks.");
   incr();
   monster_blink(i);
 }
 break;

        case MWATER3: // jellyfish
// if (random2(3) != 0) break;
 if (res_poison != 0) break;
 strcpy(info [info_lines], monam (mons_sec [monster_attacking], mons_class [monster_attacking], mons_ench [monster_attacking] [2], 0));
        strcat(info [info_lines], " stings you!");
        incr();
        poison++;
 if (sust_abil == 0 && strength > 3)
 {
  strcpy(info [info_lines], "You feel weaker.");
  incr();
  strength_ch = 1;
                strength --;
 }
        break;


        } // end of switch for special attacks.




        // use brek for level drain, maybe with beam variables, because so many creatures use it.

if (brek == 1) // energy drain:
{
lose_level();

       /*
        xp = (xp - 20 * (xl - 1) * (xl - 1) - 20 * (xl - 2) * (xl - 2));

        xl --;

//      if (xp > (20 * xl * xl) && xl < 27)
//      {
                //xl++;
                strcpy(info [info_lines], "You are now a level ");
                itoa(xl, temp_quant, 10);
                strcat(info [info_lines], temp_quant);
                strcat(info [info_lines], " ");
                strcat(info [info_lines], clasnam);
                strcat(info [info_lines], "!");
                incr();
//              message();
//              more();
        switch(clas)
        {
        case 0: // fighter
        brek = random2(6) + 5;
        hp -= brek;
        hp_max -= brek;
        brek = random2(3) + 1;
        ep -= brek;
        ep_max -= brek;
        f_abil -= 1;
        speed += .02;
        break;

        }

        rate_regen /= 1.1;

        xl_ch = 1; hp_ch = 1; ep_ch = 1; xp_ch = 1;

        brek = 0;
*/

}

}





char drained = 0;





// special weapons
if (mons_inv [monster_attacking] [hand_used] != 501 && hit == 1)
{
 switch(item_dam [mons_inv [monster_attacking] [hand_used]])
  {
   case 0: // nothing
   break;

   case 1: // flaming
   specdam = 0;
   if (res_fire != 0)
   {
    specdam = random2(damage_taken) / 2 + 1;
   }
/*   if (res_fire != 0)
   {
    specdam = random2(8) + 1;
   }*/
    if (specdam != 0)
    {
        strcpy(info [info_lines], monam (mons_sec [monster_attacking], mons_class [monster_attacking], mons_ench [monster_attacking] [2], 0));
        strcat(info [info_lines], " burns you");
        if (specdam < 3) strcat(info [info_lines], ".");
        if (specdam >= 3 && specdam < 7) strcat(info [info_lines], "!");
        if (specdam >= 7) strcat(info [info_lines], "!!");
        //    strcat(info [info_lines], " is burned.");
        incr();
        //mons_hp [monster_attacked] -= specdam;
        damage_taken += specdam;
    }
   break;

   case 2: // freezing
   specdam = 0;
   if (res_cold == 0)
   {
    specdam = random2(damage_taken) / 2 + 1;
   }
   /*   if (res_fire != 0)
   {
    specdam = random2(8) + 1;
   }*/
    if (specdam != 0)
    {
//        incr();
        strcpy(info [info_lines], monam (mons_sec [monster_attacking], mons_class [monster_attacking], mons_ench [monster_attacking] [2], 0));
        strcat(info [info_lines], " freezes you");
        if (specdam < 3) strcat(info [info_lines], ".");
        if (specdam >= 3 && specdam < 7) strcat(info [info_lines], "!");
        if (specdam >= 7) strcat(info [info_lines], "!!");
        //    strcat(info [info_lines], " is burned.");
        incr();
        //mons_hp [monster_attacked] -= specdam;
        damage_taken += specdam;
    }
   break;


   case 3: // holy wrath
   // no effect!
   break;

   case 4: // electrocution
   if (lev != 0) break; // you're not grounded
   specdam = 0;

   if (item_plus [mons_inv [monster_attacking] [hand_used]] <= 50 | item_plus [mons_inv [monster_attacking] [hand_used]] > 130 && item_plus [mons_inv [monster_attacking] [hand_used]] <= 150) break;
   if (random2(3) == 0)
   {
    strcpy(info [info_lines], "You are electrocuted!");
    incr();
    specdam += 10 + random2(15);
    item_plus [mons_inv [monster_attacking] [hand_used]] --;
   }
   damage_taken += specdam;
   break;

   case 5: // orc slaying
   // I assume you're not an orc.
   break;

   case 6: // venom
   if (random2(3) == 0 && res_poison == 0)
   {
     strcpy(info [info_lines], monam (mons_sec [monster_attacking], mons_class [monster_attacking], mons_ench [monster_attacking] [2], 0));
     strcat(info [info_lines], "'s weapon is poisoned!");
                                        poison += 2;
   }
   break;

   case 7: // protection
   break;

   case 8: if (prot_life != 0) break;
   strcpy(info [info_lines], "You feel drained...");
   drained = 1;
                        break;

  } // end of switch
} // end of special weapons

if (drained == 1) lose_level();



//incr();

if (damage_taken > 0)
{
        ouch(damage_taken, monster_attacking, 0);
        hp_ch = 1;
}


} // end of for runthru


return;
} // end of void monster_attack(int monster_attacking)


/*
int inflict_dam (void)
{
//k = item_class [te];
//p = item_type [te];
//p = property [j] [k] [0];

//return (property [item_class [x]] [item_type [1]] [0]);


return 5;
} // end of inflict_dam

*/






void you_attack(int monster_attacked)
{
float your_to_hit;
int damage_done = 0;
char hit = 0;

//your_to_hit = strength + (0.5 * dex) * (float) (random2 (100) / 100); // include fighting ability here.

if (mons_beh [monster_attacked] != 7) pet_target = monster_attacked;

your_to_hit = strength + (0.5 * dex) + (f_abil / 5); // / 100

alert2();

hunger -= 3;

if (special_wield != 0) //item_wielded != -1 && inv_class [item_wielded] == 0 && hit == 1 && inv_dam [item_wielded] > 180)
{

 switch(special_wield)
 {
  case 2:
  if (berserker != 0 | slow != 0) break;
  strcpy(info [info_lines], "A red film seems to cover your vision as you go berserk!");
  incr();
  strcpy(info [info_lines], "You feel yourself moving faster!");
  incr();
  strcpy(info [info_lines], "You feel mighty!");
  incr();
  berserker += 10 + random2(10) + random2(10);
  haste += berserker;
  might += berserker;
  strength += 5;
  max_strength += 5;
  break;
 }
}

if (item_wielded != -1)
{
 if (inv_class [item_wielded] == 0)
 {
        int ghoggl = inv_plus [item_wielded] - 50;
        if (inv_plus [item_wielded] > 130) ghoggl -= 100;
        //damage_done += inv_plus [item_wielded];

 //     ghoggl = (ghoggl / 10);

        your_to_hit += random2(ghoggl + 1); // * 3;

        //your_to_hit += inv_plus [item_wielded];
        your_to_hit += 2 * property [0] [inv_type [item_wielded]] [1];
  if (clas == 1 | clas == 3 && property [0] [inv_type [item_wielded]] [1] < 0)
        your_to_hit += property [0] [inv_type [item_wielded]] [1];
 }
 if (inv_class [item_wielded] == 11)
        {
  your_to_hit += 8;
        }
}

if (hung_state == 1)
{
 your_to_hit -= 3;
}

if (armour [5] != -1)
{
 if (clas == 1 | clas == 3)
 {
  if (inv_type [armour [5]] == 8) your_to_hit -= 1;
  if (inv_type [armour [5]] == 14) your_to_hit -= 2;
 }
 if (inv_type [armour [5]] == 14) your_to_hit --;
}
//your_to_hit *= random2(200) / 100;

your_to_hit = random2(your_to_hit);

damage = 1;

if (item_wielded != -1)
{
 if (inv_class [item_wielded] == 0)
 {
   damage = property [inv_class [item_wielded]] [inv_type [item_wielded]] [0];
 }
 if (inv_class [item_wielded] == 11)
        {
   damage = 5;
        }
}

if ((your_to_hit >= mons_ev [monster_attacked] | random2(15) == 0) | (mons_speed [monster_attacked] > 100 && random2 (20) != 0))
//if (your_to_hit - mons_ev [monster_attacked]
{
        hit = 1;
        damage_done = (int) random2(damage + f_abil / 10); // remember to include strength;
        if (might > 1) damage_done += random2(10) + 1;
 if (hung_state == 1) damage_done -= random2(5);
        //damage_done /= 2;
        if (inv_class [item_wielded] == 0)
        {
                int hoggl = inv_plus [item_wielded] - 50;
                if (inv_plus [item_wielded] > 130) hoggl -= 100;

                //damage_done += inv_plus [item_wielded];

//              hoggl = (hoggl % 10);

                damage_done += random2(hoggl + 1);

                //damage_done /= ((mons_AC [monster_attacked] / property [inv_class [item_wielded]] [inv_type [item_wielded]] [2]) + 1);
        }// else
  else (damage_done -= random2(5));
        //       damage_done /= (random2 (mons_AC [monster_attacked]) + 1);

        damage_done -= random2 (mons_AC [monster_attacked] + 1);

        //damage_done /= (random2 (mons_AC [monster_attacked]) + 1);
        //damage_done *= 2;
//      damage_done -= random2(mons_AC [monster_attacked]);

        if (damage_done < 0) damage_done = 0;
        mons_hp [monster_attacked] -= damage_done;

        if (mons_hp [monster_attacked] <= 0)
        {
                thing_thrown = 1;
                //info_lines --; // so it just prints - You kill the ...
                killer = 1;
                monster_die(monster_attacked);
                if (mons_class [monster_attacked] == 32)
                {
                        strcpy(info [info_lines], "You hit the giant spore.");
                        incr();
                }
                return;
        }

        if (damage_done < 1 && mons_ench [monster_attacked] [2] != 6)
        {
        hit = 1;
//      strcpy(info [info_lines], "You miss the ");
        strcpy(info [info_lines], "You hit ");
        strcat(info [info_lines], monam (mons_sec [monster_attacked], mons_class [monster_attacked], mons_ench [monster_attacked] [2], 1));
        strcat(info [info_lines], ", but it appears unharmed.");
        incr();
        }

} else
        {
        hit = 0;
        strcpy(info [info_lines], "You miss ");
        strcat(info [info_lines], monam (mons_sec [monster_attacked], mons_class [monster_attacked], mons_ench [monster_attacked] [2], 1));
        strcat(info [info_lines], ".");
        incr();
        }

if (hit == 1 && damage_done >= 1 | (hit == 1 && damage_done < 1 && mons_ench [monster_attacked] [2] == 6))
        {
        strcpy(info [info_lines], "You hit ");
//      strcat(info [info_lines], gmon_name [mons_class [monster_attacked]]);
        strcat(info [info_lines], monam (mons_sec [monster_attacked], mons_class [monster_attacked], mons_ench [monster_attacked] [2], 1));
        if (damage_done < 5) strcat(info [info_lines], ".");
 if (damage_done >= 5 && damage_done < 12) strcat(info [info_lines], "!");
 if (damage_done >= 12 && damage_done < 21) strcat(info [info_lines], "!!");
 if (damage_done >= 21) strcat(info [info_lines], "!!!");
 incr();
 }

if (mons_class [monster_attacked] == 35) weapon_acid(5);

int specdam = 0;
// remember, damage_done is still useful!


if (item_wielded != -1 && inv_class [item_wielded] == 0 && hit == 1 && inv_dam [item_wielded] <= 180)
{
  switch(inv_dam [item_wielded] % 30)
  {
   case 0: // nothing
   break;

   case 1: // flaming
   specdam = 0;
   if (mons_res_fire(mons_class [monster_attacked]) == 0 && (mons_inv [monster_attacked] [2] == 501 | item_dam [mons_inv [monster_attacked] [2]] % 30 != 2))
   {
    specdam = random2(damage_done) / 2 + 1;
   }
   if (mons_res_fire(mons_class [monster_attacked]) == -1 && (mons_inv [monster_attacked] [2] == 501 | item_dam [mons_inv [monster_attacked] [2]] % 30 != 2))
   {
    specdam = random2(damage_done) + 1;
   }
    if (specdam != 0)
    {
        strcpy(info [info_lines], "You burn ");
        strcat(info [info_lines], monam (mons_sec [monster_attacked], mons_class [monster_attacked], mons_ench [monster_attacked] [2], 1));
        if (specdam < 3) strcat(info [info_lines], ".");
        if (specdam >= 3 && specdam < 7) strcat(info [info_lines], "!");
        if (specdam >= 7) strcat(info [info_lines], "!!");
        //    strcat(info [info_lines], " is burned.");
        incr();
        mons_hp [monster_attacked] -= specdam;
    }
   break;

   case 2: // freezing
   specdam = 0;
   if (mons_res_cold(mons_class [monster_attacked]) == 0 && (mons_inv [monster_attacked] [2] == 501 | item_dam [mons_inv [monster_attacked] [2]] % 30 != 3))
   {
    specdam = random2(damage_done) / 2 + 1;
   }
   if (mons_res_cold(mons_class [monster_attacked]) == -1 && (mons_inv [monster_attacked] [2] == 501 | item_dam [mons_inv [monster_attacked] [2]] % 30 != 3))
   {
    specdam = random2(damage_done) + 1;
   }
    if (specdam != 0)
    {
        strcpy(info [info_lines], "You freeze ");
        strcat(info [info_lines], monam (mons_sec [monster_attacked], mons_class [monster_attacked], mons_ench [monster_attacked] [2], 1));
        if (specdam < 3) strcat(info [info_lines], ".");
        if (specdam >= 3) strcat(info [info_lines], "!");
        if (specdam >= 7) strcat(info [info_lines], "!");
        //    strcat(info [info_lines], " is burned.");
        incr();
        mons_hp [monster_attacked] -= specdam;
    }
   break;

   case 3: // holy wrath
   specdam = 0;
   switch(mons_holiness(mons_class [monster_attacked]))
   {
    case -1:
//    strcpy(info [info_lines], "
      damage_done -= 5 + random2(5);
    break;

    case 1:
      specdam += random2(damage_done) + 1;
    break;

    case 2:
      specdam += random2(damage_done) * 1.5 + 1; // does * 1.5 do anything?
    break;
   }
//   if (specdam > 0)
//   {
//    strcpy(info [info_lines], "
    mons_hp [monster_attacked] -= specdam;
//   }
   break;


   case 4: // electrocution
   specdam = 0;
   if (mons_flies(mons_class [monster_attacked]) == 2) break;
   if (inv_plus [item_wielded] <= 50 | inv_plus [item_wielded] > 130 && inv_plus [item_wielded] <= 150) break;
   if (random2(3) == 0)
   {
    strcpy(info [info_lines], "There is a sudden explosion of sparks!");
    incr();
    specdam += 10 + random2(15);
    inv_plus [item_wielded] --;
   }
   mons_hp [monster_attacked] -= specdam;
   break;

   case 5: // orc slaying

   if (mons_class [monster_attacked] == 14 | mons_class [monster_attacked] == 52 | mons_class [monster_attacked] == 54 | mons_class [monster_attacked] == 55)
   {
     mons_hp [monster_attacked] -= 1 + random2(damage_done);
   }
   break;

   case 6: // venom
   if (random2(3) == 0) poison_monster(monster_attacked, 0);
   break;

//   case 7: // protection

   case 8:
   if (mons_holiness(mons_class [monster_attacked]) > 0 | random2(4) != 0) break;
   strcpy(info [info_lines], "You drain ");
   strcat(info [info_lines], monam (mons_sec [monster_attacked], mons_class [monster_attacked], mons_ench [monster_attacked] [2], 1));
   strcat(info [info_lines], "!");
                 incr();
   mons_HD [monster_attacked] --;
   mons_hp_max [monster_attacked] -= 3 + random2(6);
   mons_hp [monster_attacked] -= 3 + random2(6);
   if (mons_hp [monster_attacked] >= mons_hp_max [monster_attacked]) mons_hp [monster_attacked] = mons_hp_max [monster_attacked];
   if (mons_HD [monster_attacked] <= 0) mons_hp [monster_attacked] = 0;
                        break;

  } // end switch


}


if (mons_class [monster_attacked] == 106) // hydra
{
 if (item_wielded != -1 && damage_type(inv_class [item_wielded], inv_type [item_wielded]) == 1)
 {
  if (random2(2) != 0 | damage_done < 4) goto mons_dies;
  strcpy(info [info_lines], "You ");
  switch(random2(4))
  {
   case 0: strcat(info [info_lines], "slice"); break;
   case 1: strcat(info [info_lines], "lop"); break;
   case 2: strcat(info [info_lines], "chop"); break;
   case 3: strcat(info [info_lines], "hack"); break;
  }
  strcat(info [info_lines], " one of ");
  strcat(info [info_lines], monam (mons_sec [monster_attacked], mons_class [monster_attacked], mons_ench [monster_attacked] [2], 1));
  strcat(info [info_lines], "'s heads off.");
  incr();
  mons_sec [monster_attacked] --;
  if (mons_sec [monster_attacked] <= 0)
  {
    mons_hp [monster_attacked] = -1;
    goto mons_dies;
  }
  if (inv_class [item_wielded] == 0 && inv_dam [item_wielded] % 30 == 1) goto mons_dies; // cauterised
  if (mons_sec [monster_attacked] >= 18) goto mons_dies;
  strcpy(info [info_lines], "It grows two more!");
  incr();
  mons_sec [monster_attacked] += 2;
 }
}
// remember, the hydra function sometimes skips straight to mons_dies

mons_dies : if (mons_hp [monster_attacked] <= 0)
{
                thing_thrown = 1;
                //info_lines --; // so it just prints - You kill the ...
                killer = 1;
                monster_die(monster_attacked);
                /*if (mons_class [monster_attacked] == 32)
                {
                        strcpy(info [info_lines], "You hit the giant spore.");
                        incr();
                }*/
                return;
}
//}


if (mons_hp [monster_attacked] <= 0)
{
                thing_thrown = 1;
                //info_lines --; // so it just prints - You kill the ...
                killer = 1;
                monster_die(monster_attacked);
/*              if (mons_class [monster_attacked] == 32)
                {
                         strcpy(info [info_lines], "You hit the giant spore.");
                         incr();
                }*/
                return;
}



/*
mons_hp [monster_attacked] -= damage_done;

if (mons_hp [monster_attacked] <= 0)
{
        thing_thrown = 1;
        info_lines --; // so it just prints - You kill the ...
        monster_die(monster_attacked);

}
*/
//message();


if (damage_done >= 1 && item_wielded != -1 && inv_class [item_wielded] == 0 && inv_type [item_wielded] > 12 && inv_type [item_wielded] < 17) //       18??
{
        if (random2 (5) == 0)
        {
//              strcpy(info [info_lines], "Your ");
                item_name(inv_class [item_wielded], inv_type [item_wielded], inv_dam [item_wielded], inv_plus [item_wielded], inv_quant [item_wielded], inv_ident [item_wielded], 4, str_pass);
                strcpy(info [info_lines], str_pass);
                strcat(info [info_lines], " breaks!");
                incr();
  unwield_item(item_wielded);
                inv_quant [item_wielded] --;
                item_wielded = -1;
        }
}

if (invis != 0 && mons_beh [monster_attacked] == 0) mons_beh [monster_attacked] = 1;

if (invis != 0 && random2(2) == 0)
{
mons_targ_1_x [monster_attacked] = x_pos;
mons_targ_1_y [monster_attacked] = y_pos;
mons_beh [monster_attacked] = 1;
}


if (item_wielded != -1)
{
        if (inv_class [item_wielded] == 0)
        {
                time_taken *= (float) ((float) property [inv_class [item_wielded]] [inv_type [item_wielded]] [2] / 10);
//              time_taken *= (property [inv_class [item_wielded]] [inv_type [item_wielded]] [2] / 10);
        }
 if (inv_class [item_wielded] == 11)    time_taken *= (float) 1.2;
}


//mons_hp [monster_attacked] += 10;

if (mons_beh [monster_attacked] == 3) mons_beh [monster_attacked] = 1;

//strcpy(info [info_lines], "You attacked.");
//incr();

return;
}







/*

void monster_fight(int monster_attacking, int monster_attacked)
{
//float mons_to_hit;
damage_taken = 0;
char hit = 0;
//char visibl = 0;
char orig_inflin = info_lines;



//mons_to_hit = mons_HD [monster_attacking]; // not finished yet.

for (runthru = 0; runthru < 4; runthru ++)
{

if (mondamage(mons_class [monster_attacking], runthru) == 0) break;
//gmon_att [mons_class [monster_attacking]] [runthru] == 0) break;

damage_taken = 0;


mons_to_hit = 20 + mons_HD [monster_attacking] * 3;  //strength + (0.5 * dex) + (f_abil);

if (mons_inv [monster_attacking] [0] != 501)
{
        if (item_plus [mons_inv [monster_attacking] [0]] - 50 > 130)
        {
                mons_to_hit += item_plus [mons_inv [monster_attacking] [0]] - 150;
        } else
              {
                mons_to_hit += (item_plus [mons_inv [monster_attacking] [0]] - 50);// / 10;
              }



//      mons_to_hit += item_plus [mons_inv [monster_attacking] [0]];
        mons_to_hit += 2 * property [0] [item_type [mons_inv [monster_attacking] [0]]] [1];
}

//if (inv_class [item_wielded] == 0) mons_to_hit += inv_plus [item_wielded];
mons_to_hit *= random2(200) / 100;
/*
//if (mons_to_hit >= (evasion - 6)) //&& mons_to_hit < evasion && shield_armour != -1) // && random2(6) != 0)
if (mons_to_hit < evasion)
{
strcpy(info [info_lines], "You block the ");
strcat(info [info_lines], gmon_name [mons_class [monster_attacking]]);
if (mons_inv [monster_attacking] [0] != 255)
        {
        strcat(info [info_lines], "'s ");
        strcat(info [info_lines], fake_name [mmov_x]);
        }
strcat(info [info_lines], " on your shield!");
incr();
return;
}
* /

if (mons_to_hit >= mons_ev [monster_attacked])// | random2(6) == 0) // mons_ev [monster_attacked])
//if (random2(20) + mons_to_hit >= 20 - (10 - AC)) // hit roll
{
hit = 1;

//te = mons_inv [monster_attacking] [0];

//k = item_class [te];
//p = item_type [te];
//j = property [k] [p] [0];
//o = mons_class [monster_attacking];
//k = gmon_att [o] [runthru];


if (mons_inv [i] [0] != 501)
{
//damage_taken = random2(mmov_x);
damage_taken = random2(property [item_class [mons_inv [i] [0]]] [item_type [mons_inv [i] [0]]] [0]);
//if (item_plus [mons_inv [i] [0]] > 80) damage_taken -= 100;
//damage_taken += item_plus [mons_inv [i] [0]];
        if (item_plus [mons_inv [monster_attacking] [0]] > 130)
        {
                damage_taken += item_plus [mons_inv [monster_attacking] [0]] - 150;
        } else
              {
                damage_taken += (item_plus [mons_inv [monster_attacking] [0]] - 50);// % 10;
              }

damage_taken -= 1;
}

damage_taken += random2(mondamage(mons_class [monster_attacking], runthru)) + 1;
//gmon_att [mons_class [monster_attacking]] [runthru]) + 1;
//damage_taken /= (random2 (AC) + 1); // / 3
//damage_taken *= 2;

//if (mons_inv [monster_attacking] [0] != 501)
//{
//      damage_taken /= ((random2 (AC) / property [item_class [mons_inv [i] [0]]] [item_type [mons_inv [i] [0]]] [2]) + 1);
//} else
//      damage_taken /= (random2 (AC) + 1);

        damage_taken -= random2(mons_AC [monster_attacked]);

if (damage_taken < 1) damage_taken = 0;
mons_hp [monster_attacked] -= (int) damage_taken;


} // end of if mons_to_hit >= evasion
 else
// if (visibl == 1)
// {
        hit = 0;
//      strcpy(info [info_lines], "The ");
        //} else
        //strcat(info [info_lines], "The ");

        strcpy(info [info_lines], monam (mons_sec [monster_attacking], mons_class [monster_attacking], mons_ench [monster_attacking] [2], 0)); //gmon_name [mons_class [monster_attacking]]);
        strcat(info [info_lines], " misses ");
        strcat(info [info_lines], monam (mons_sec [monster_attacked], mons_class [monster_attacked], mons_ench [monster_attacked] [2], 1)); //gmon_name [mons_class [monster_attacked]]);
        strcat(info [info_lines], ".");
        incr();
// }


if (damage_taken < 1 && hit == 1)
{
//if (78 - strlen(info [info_lines]) < strlen(gmon_name [mons_class [monster_attacking]]) + 17)
//      {
        //incr();
//      strcpy(info [info_lines], "The ");
        //} else
        //strcat(info [info_lines], "The ");

        strcat(info [info_lines], monam (mons_sec [monster_attacking], mons_class [monster_attacking], mons_ench [monster_attacking] [2], 0)); //gmon_name [mons_class [monster_attacking]]);
        strcat(info [info_lines], " hits ");
        strcat(info [info_lines], monam (mons_sec [monster_attacked], mons_class [monster_attacked], mons_ench [monster_attacked] [2], 1));
//); //gmon_name [mons_class [monster_attacked]]);
        strcat(info [info_lines], " but doesn't do any damage.");

        incr();
}



if ((int) damage_taken >= 1)
{
//if (78 - strlen(info [info_lines]) < strlen(gmon_name [mons_class [monster_attacking]]) + 15)
//      {
//      incr();
//      strcpy(info [info_lines], "The ");
//      } else
//      strcpy(info [info_lines], "The ");

        strcpy(info [info_lines], monam (mons_sec [monster_attacking], mons_class [monster_attacking], mons_ench [monster_attacking] [2], 0)); //gmon_name [mons_class [monster_attacking]]);
        strcat(info [info_lines], " hits ");
        strcat(info [info_lines], monam (mons_sec [monster_attacked], mons_class [monster_attacked], mons_ench [monster_attacked] [2], 1)); //gmon_name [mons_class [monster_attacked]]);
//      strcat(info [info_lines], " ");


        if (mons_inv [monster_attacking] [0] != 501)
        {
        strcat(info [info_lines], " with ");
        item_name(item_class [mmov_x], item_type [mmov_x], item_dam [mmov_x], item_plus [mmov_x], item_quant [mmov_x], item_ident [mmov_x], 7, str_pass);
        strcat(info [info_lines], str_pass);
        }
        strcat(info [info_lines], "!"); // put something for '.', '!', '!!' etc depending on what % of damage you took.

        incr();

        // special attacks:

        brek = 0;

        switch(mons_class [monster_attacking])
        {
        case 0: // giant ant
        if ((damage_taken >= 4 && random2(4) == 0) | random2(10) == 0)
        {
        strcpy(info [info_lines], "The giant ant stings you!");
        incr();
        poison++;
        }
        break;

        case 2: // cockatrice.
        break;

        case 10: // killer bee
        if ((damage_taken >= 3 && random2(4) == 0) | random2(10) == 0)
        {
        strcpy(info [info_lines], "The killer bee stings you!");
        incr();
        poison += 2;
        }
        break;

        case 18: // snake
        if ((damage_taken >= 3 && random2(4) == 0) | random2(10) == 0)
        {
        strcpy(info [info_lines], "The snake digs its fangs into your flesh!");
        incr();
        poison++;
        }
        break;

        case 48: // wraith
        if ((damage_taken >= 6 && random2(5) == 0) | random2(20) == 0)
        {
        strcpy(info [info_lines], "You feel a terrible chill...");
        incr();
        brek = 1;
        }
        break;



        } // end of switch for special attacks.

        // use brek for level drain, maybe with beam variables, because so many creatures use it.

if (brek == 1) // energy drain:
{
//mons_HD [monster_attacked] --;



}

}

//incr();

} // end of for runthru


if (mons_hp [monster_attacked] <= 0 | mons_HD [monster_attacked] <= 0)
{
        killer = 2;
        monster_die (monster_attacked);
}



if (mons_x [monster_attacking] > x_pos - 7 && mons_x [monster_attacking] < x_pos + 7 && mons_y [monster_attacking] > y_pos - 7 && mons_y [monster_attacking] < y_pos + 7)
{
        if (show [mons_x [monster_attacking] - x_pos + 7] [mons_y [monster_attacking] - y_pos + 7] != 0)
                return;
}

info_lines = orig_inflin;




return;
} // end of void monster_attack(int monster_attacking)




*/














































void monster_die(int monster_killed)
{

int dmi; // dead monster's inventory

/*
strcpy(info [info_lines], "The ");
strcat(info [info_lines], gmon_name [mons_class [monster_killed]]);
strcat(info [info_lines], " dies.");
incr();
*/

if (prev_targ == monster_killed) prev_targ = 500;

if (mons_class [monster_killed] == 32)
{
        if (mons_hp [monster_killed] < 1 && mons_hp [monster_killed] > -15)
        {
                return;
        }
}

if (mons_class [monster_killed] == 21)
{
//strcpy(info [info_lines], "The ");
strcpy(info [info_lines], monam (mons_sec [monster_killed], mons_class [monster_killed], mons_ench [monster_killed] [2], 0));
strcat(info [info_lines], " dissipates!"); // <- spelling?
if (mons_near(monster_killed) == 1) incr();
if (killer == 1 | killer == 3)
{
 xp += exper_value(mons_class [monster_killed], mons_HD [monster_killed], mons_hp_max [monster_killed]);
 level_change();
}
place_cloud(101, mons_x [monster_killed], mons_y [monster_killed], 2 + random2(4));
goto out_of_switch;
} // end fire vortex

switch(killer)
{
case 1: // You kill in combat.
strcpy(info [info_lines], "You kill ");
strcat(info [info_lines], monam (mons_sec [monster_killed], mons_class [monster_killed], mons_ench [monster_killed] [2], 1));  //gmon_name [mons_class [monster_killed]]);
strcat(info [info_lines], "!");
incr(); // put experience points here.
xp += exper_value(mons_class [monster_killed], mons_HD [monster_killed], mons_hp_max [monster_killed]);
level_change();
break;

case 2: // Monster kills in combat
//if (mons_x [] > x_pos - 7 && mons_x [i] < x_pos + 7 && mons_y [i] > y_pos - 7 && mons_y [i] < y_pos + 7)
//{
//if (show [mons_x [i] - x_pos + 7] [mons_y [i] - y_pos + 7] != 0)
if (mons_near(monster_killed) == 1)
{
//      strcpy(info [info_lines], "The ");
//      strcpy(info [info_lines], monam (mons_sec [i], mons_class [i], mons_ench [i] [2], 0)); //gmon_name [mons_class [i]]);
//      strcat(info [info_lines], " kills ");
        strcpy(info [info_lines], monam (mons_sec [monster_killed], mons_class [monster_killed], mons_ench [monster_killed] [2], 0)); //gmon_name [mons_class [monster_killed]]);
        strcat(info [info_lines], " dies!");
        incr();
}
//}
break;

case 3: // You kill by missile or beam
strcpy(info [info_lines], "The ");
strcat(info [info_lines], beam_name);//gmon_name [mons_class [i]]);
strcat(info [info_lines], " kills ");
strcat(info [info_lines], monam (mons_sec [monster_killed], mons_class [monster_killed], mons_ench [monster_killed] [2], 1)); // gmon_name [mons_class [monster_killed]]);
strcat(info [info_lines], "!");
incr();
xp += exper_value(mons_class [monster_killed], mons_HD [monster_killed], mons_hp_max [monster_killed]);
level_change();
break;

case 4: // Monster kills by missile or beam
strcpy(info [info_lines], "The ");
strcat(info [info_lines], beam_name);//gmon_name [mons_class [i]]);
strcat(info [info_lines], " kills ");
strcat(info [info_lines], monam (mons_sec [monster_killed], mons_class [monster_killed], mons_ench [monster_killed] [2], 1)); //gmon_name [mons_class [monster_killed]]);
strcat(info [info_lines], "!");
incr();
break;

case 5: // Monster killed by trap/inanimate thing/itself/poison not from you
//strcpy(info [info_lines], "The ");
if (mons_near(monster_killed))
{
strcpy(info [info_lines], monam (mons_sec [monster_killed], mons_class [monster_killed], mons_ench [monster_killed] [2], 0)); //gmon_name [mons_class [i]]);
strcat(info [info_lines], " dies!");
incr();
}
break;

case 6: // Monster doesn't die, just goes back to wherever it came from
// This must only be called by monsters running out of time (or abjuration),
//  because it uses the beam variables!
if (mons_near(monster_killed))
{
strcpy(info [info_lines], monam (mons_sec [monster_killed], mons_class [monster_killed], mons_ench [monster_killed] [2], 0)); //gmon_name [mons_class [i]]);
strcat(info [info_lines], " disappears in a puff of smoke!");
incr();
// beam_type = random2(3) + 5;
// beam_range = 1 + random2(3);
// target_x = mons_x [monster_killed];
// target_y = mons_y [monster_killed];
 place_cloud(random2(3) + 105, mons_x [monster_killed], mons_y [monster_killed], 1 + random2(3));
// place_cloud();
}
for (dmi = 7; dmi >= 0; dmi --) // takes whatever it's carrying back home
        {
        if (mons_inv [monster_killed] [dmi] != 501)
        {
   item_quant [mons_inv [monster_killed] [dmi]] = 0;
        }
        mons_inv [monster_killed] [dmi] = 501;
        }
break;

} // end of switch


out_of_switch : if (mons_class [monster_killed] == 38) // mummy!
{
  if (killer == 1 | killer == 3)
  {
    if (curse_an_item(1, 0) == 1)
    {
      strcpy(info [info_lines], "You feel nervous for a moment...");
      incr();
    }
  }
}

if (mons_class [monster_killed] == 56)
{
 dmi = monster_killed;
 while(mons_class [dmi] != 19)
        {
  dmi--;
        }

 if (mons_HD [dmi] >= 3)
 {
                mons_HD [dmi] -= 2;
  mons_hp_max [dmi] -= 8 + random2(8);
  mons_hp [dmi] -= random2(8) + 8;
         if (mons_hp_max [dmi] <= 0) mons_hp_max [dmi] = 1;
  if (mons_hp [dmi] > mons_hp_max [dmi]) mons_hp [dmi] = mons_hp_max [dmi];
  if (mons_hp [dmi] <= 0) mons_hp [dmi] = 1;
 }
}


if (mons_class [monster_killed] == 19 | mons_class [monster_killed] == 56)
{
 dmi = monster_killed;

        while(mons_class [dmi + 1] == 56)
        {
  dmi ++;
  mons_class [dmi] = -1;
  if ((grid [mons_x [dmi]] [mons_y [dmi]] >= 50 && grid [mons_x [dmi]] [mons_y [dmi]] < 100) | (grid [mons_x [dmi]] [mons_y [dmi]] >= 150 && grid [mons_x [dmi]] [mons_y [dmi]] < 200)) grid [mons_x [dmi]] [mons_y [dmi]] -= 50;
        }

}

//thing_thrown = 0;

/*for (j = 0; j < 8; j ++)
        {
/*
        for (dmi = 0; dmi < 300; dmi++)
        {
                if (itc > it_no) break;

                if (item_quant [dmi] != 0)
                {
                itc++;

                if (dmi != mons_inv [monster_killed] [j] && (item_class [dmi] == 1 | item_class [dmi] == 4 | item_class [dmi] == 6 | item_class [dmi] == 8 | item_class [dmi] == 9) && (item_x [dmi] == mons_x [monster_killed]) && item_y [dmi] == mons_y [monster_killed] && item_class [mons_inv [monster_killed] [j]] == item_class [dmi] && item_type [mons_inv [monster_killed] [j]] == item_type [dmi] && item_plus [mons_inv [monster_killed] [j]] == item_plus [dmi] && item_dam [mons_inv [monster_killed] [j]] == item_dam [dmi] && item_quant [dmi] > 0)
                {
                if (strcmp(fake_name [mons_inv [monster_killed] [j]], fake_name [dmi]) == 0); // && mons_x [monster_killed] == i
                {
                        item_quant [dmi] += item_quant [mons_inv [monster_killed] [j]];
                        item_quant [mons_inv [monster_killed] [j]] = 0;
                        //item_quant [mons_inv [monster_killed] [j]] = 0;
                        //return; //break; //return;
                }
                }
                } // end of if item_quant
        }

* /

        if (mons_inv [monster_killed] [j] != 501)
        {
                if (igrid [mons_x [monster_killed]] [mons_y [monster_killed]] == 501)
                // this bit is wrong, BTW:
                //if (mons_inv [monster_killed] [j] != 501)
                {
                        igrid [mons_x [monster_killed]] [mons_y [monster_killed]] = mons_inv [monster_killed] [j];
                } else
                {
                        int huj = igrid [mons_x [monster_killed]] [mons_y [monster_killed]];
                        igrid [mons_x [monster_killed]] [mons_y [monster_killed]] = mons_inv [monster_killed] [j];
                        item_link [mons_inv [monster_killed] [j]] = huj;
                }
        }


        mons_inv [monster_killed] [j] = 501;
        //itc = 0;
        }
  */

  monster_drop_ething(monster_killed);

  for (j = 0; j < 3; j++)
        {
                mons_ench [monster_killed] [j] = 0;
        }

        mons_ench_1 [monster_killed] = 0;

        //itc = 0;
no_mons --;
mons_class [monster_killed] = -1;
mons_hp [monster_killed] = 0;
mons_hp_max [monster_killed] = 0;
mons_HD [monster_killed] = 0;
mons_AC [monster_killed] = 0;
mons_ev [monster_killed] = 0;

//if (grid [mons_x [monster_killed]] [mons_y [monster_killed]] >= 50) grid [mons_x [monster_killed]] [mons_y [monster_killed]] -= 50;

//while (grid [mons_x [monster_killed]] [mons_y [monster_killed]] >= 50)
//{
        //grid [mons_x [monster_killed]] [mons_y [monster_killed]] -= 50;

if ((grid [mons_x [monster_killed]] [mons_y [monster_killed]] >= 50 && grid [mons_x [monster_killed]] [mons_y [monster_killed]] < 100) | (grid [mons_x [monster_killed]] [mons_y [monster_killed]] >= 150 && grid [mons_x [monster_killed]] [mons_y [monster_killed]] < 200))
       grid [mons_x [monster_killed]] [mons_y [monster_killed]] -= 50;

//}

//mons_place(); don't put this here

//monster_grid();

brek = 1;
viewwindow();

for (dmi = 0; dmi < MNST; dmi ++)
{
        if (mons_hit [dmi] == monster_killed) mons_hit [dmi] = MHITNOT;
}

if (pet_target == monster_killed) pet_target = MHITNOT;

return;
}



void lose_level(void)            // NEED To adjust a lot of numbers here....
{                               // To keep in line with gain level

//while (xp > exp_needed(xl + 2) && xl < 27) //(20 * xl * xl)) // && xl < 27)

//if (xl == 1) ouch

        if (xl == 1)
 {
    ouch(-9999, 0, 14);
          //cprintf("You've run out of xp, sorry!");
//              exit(0);
 }
// because xp is unsigned long, if it's going to be -ve must die straightaway.

//      xp = (xp - 20 * (xl - 1) * (xl - 1) - 20 * (xl - 2) * (xl - 2));
 xp = exp_needed(xl - 1) + 1;
        xl --;

//      if (xp > (20 * xl * xl) && xl < 27)
//      {
                //xl++;
                strcpy(info [info_lines], "You are now a level ");
                itoa(xl, temp_quant, 10);
                strcat(info [info_lines], temp_quant);
                strcat(info [info_lines], " ");
                strcat(info [info_lines], clasnam);
                strcat(info [info_lines], "!");
                incr();
//              message();
//              more();
        switch(clas)
        {

case 0: // fighter
        brek = random2(5) + 4;
 ouch(brek, 0, 14);
        hp_max -= brek;
        brek = 1; //random2(3);
        ep -= brek;
        ep_max -= brek;
        f_abil -= 10; // 15
        mag_abil -= 3;
 thr_abil -= 5;
 res_magic -= 3;
 spell_levels++;
//      speed -= .02;                         No change in speed???
break;

case 1: // wizard
        brek = random2(4) + 3;
 ouch(brek, 0, 14);
        hp_max -= brek;
        brek = random2(2) + 2;
        ep -= brek;
        ep_max -= brek;
        f_abil -= 3;
        mag_abil -= 7;
 thr_abil -= 2;
 res_magic -= 5;
 spell_levels -= 5 - brek;
//      speed -= .02;
break;

case 3: // thief
        brek = random2(4) + 4;
 ouch(brek, 0, 14);
        hp_max -= brek;
        brek = 1 + random2(2); //random2(3);
        ep -= brek;
        ep_max -= brek;
        f_abil -= 7; // 15
        mag_abil -= 4;
 thr_abil -= 8;
 res_magic -= 3;
 spell_levels--;
break;

case 2: // priest
        brek = random2(4) + 4;
 ouch(brek, 0, 14);
        hp_max -= brek;
        brek = random2(2) + 1;
        ep -= brek;
        ep_max -= brek;
        f_abil -= 6; // 15
        mag_abil -= 5;
 thr_abil -= 1;
 res_magic -= 5;
 spell_levels -= 1;
break;


        }

if (spell_levels < -40) spell_levels = -40;

//      rate_regen /= 1.1; // Look at this!!!

        xl_ch = 1; hp_ch = 1; ep_ch = 1; xp_ch = 1;

        brek = 0;




}




char *monam(unsigned char mons_sc, int mons_cla, char mons_e, char desc)
{

char gmo_n [40];

//strcpy(gmo_n, moname(mons_cla, mons_e, see_invis, desc));

if (mons_cla >= MLAVA0 && mons_sc == 1)
{
        switch(desc)
        {
        case 0:
        case 2: strcpy(gmo_n, "It"); break;
        case 1:
        case 3: strcpy(gmo_n, "it"); break;
        }
//      strcpy(gmo_n, glog);
        return gmo_n;
}


moname(mons_cla, mons_e, see_invis, desc, gmo_n);

if (mons_cla == 25 | mons_cla == 51) // | mons_clas == Z
{
        if (mons_e == 6 && see_invis == 0)
        {
                return gmo_n;
        }

        char gmo_n2 [40];

        moname(mons_sc, mons_e, see_invis, 10, gmo_n2);

        strcat(gmo_n, gmo_n2);

        strcat(gmo_n, " zombie");

        return gmo_n;
}


return gmo_n;

}



void incr(void)
{

//info_lines++;

/*
char oggy;

incr();

oggy = incrline();
if (oggy == 77) relay_message();
more();
*/
//return;

info_lines++;
scrloc++;
scruggle = 1;

relay_message();

//char oggy;

//incr();

//oggy = incrline();
/*
if (info_lines >= 75)
{
        relay_message();
        more();
        info_lines = 0;
        //zeroinf();
}
*/
return;

}




char mons_near(unsigned char monst)
{
// need to change to "mons_vis(char)" to include invis etc.

//losight(show, grid, x_pos, y_pos);

if (mons_x [monst] > x_pos - 7 && mons_x [monst] < x_pos + 7 && mons_y [monst] > y_pos - 7 && mons_y [monst] < y_pos + 7)
{
//      viewwindow();
//        brek = 0;
        if (show [mons_x [monst] - x_pos + 7] [mons_y [monst] - y_pos + 7] != 0)
                return 1;
}

return 0;

} // end of char mons_near(char)


char see_grid(unsigned char grx, unsigned char gry)
{
// need to change to "mons_vis(char)" to include invis etc.

//losight(show, grid, x_pos, y_pos);

if (grx > x_pos - 7 && grx < x_pos + 7 && gry > y_pos - 7 && gry < y_pos + 7)
{
//      viewwindow();
//        brek = 0;
        if (show [grx - x_pos + 7] [gry - y_pos + 7] != 0)
                return 1;
}

return 0;

} // end of char mons_near(char)




void mesclr(void)
{
// set_cursor_type(NO_CURSOR);
        _setcursortype(_NOCURSOR);
 window(2,18,78,25);
 clrscr();
 scrloc = 0;
        _setcursortype(_NORMALCURSOR);
        window(2,2,35,18);
        gotoxy(17,8);
}




void relay_message(void)
{
//char eggl [5];

        if (info_lines > 0) running = 0;
        scrloc = message(info, info_lines, scrloc);
//      if (info_lines >= 20)
 info_lines = 0;
}





void ouch(int dam, char death_source, char death_type)
{
char point_print [10];
        hp_ch = 1;
 if (deaths_door > 0 && death_type != 4 && death_type != 5 && death_type != 6)
 {
  return;
 }

 if (dam > -9000)
        {
                hp -= dam;
                if (hp > 0) return;
        }

points = 0;
points += gp;
points += xp;
if (death_type == 12) points += points / 2;
if (death_type == 11) points += points / 10;
itoa(points, point_print, 10);
strcpy(death_string, point_print);
if (points < 10000000) strcat(death_string, " ");
if (points < 1000000) strcat(death_string, " ");
if (points < 100000) strcat(death_string, " ");
if (points < 10000) strcat(death_string, " ");
if (points < 1000) strcat(death_string, " ");
if (points < 100) strcat(death_string, " ");
if (points < 10) strcat(death_string, " ");
/*for (i = 0; i < 12 - strlen(death_string); i ++)
{
 strcat(death_string, " ");
}*/

        strcat(death_string, your_name);
        strcat(death_string, "-");
        death_string [strlen(death_string)] = clasnam [0];

itoa(xl, point_print, 10);
strcat(death_string, point_print);
//if (strlen(point_print) == 1) strcat(death_string, " ");

//      strcat(death_string, ", killed by ");
 switch(death_type)
        {

        case 0: // monster
                strcat(death_string, " was killed by a");
                strcat(death_string, monam(mons_sec [death_source], mons_class [death_source], 0, 99));
 break;

        case 1: // poison
//              if (dam == -9999) strcat(death_string, "an overload of ");
                strcat(death_string, " was killed by a lethal dose of poison");
 break;

        case 2: // cloud
                strcat(death_string, " was killed by a cloud");
        break;

        case 3: // beam
                strcat(death_string, " was killed by a "); // an?
                strcat(death_string, beam_name);
 break;

 case 4: // death's door running out
  strcat(death_string, " ran out of time");
 break;

 case 5: // falling into lava
  strcat(death_string, " took a swim in molten lava");
 break;

 case 6: // falling into water
  strcat(death_string, " drowned");
 break;

// these three are probably only possible if you wear a ring of >= +3 ability,
//  get drained to 3, then take it off, or have a very low abil and wear a -ve ring.
 case 7: // lack of intelligence
  strcat(death_string, " died of stupidity");
        break;
 case 8: // lack of str
  strcat(death_string, " died of muscular atrophy");
        break;
 case 9: // lack of dex
  strcat(death_string, " died of clumsiness"); // crappy message
        break;

 case 10:
  strcat(death_string, " was killed by a trap");
 break;

 case 11:
  strcat(death_string, " got out of the dungeon alive.");
 break;

 case 12:
  strcat(death_string, " escaped with the Orb.");
 break;

 case 13:
  strcat(death_string, " quit");
 break;

 case 14:
  strcat(death_string, " was drained of all life");
 break;

 case 15:
  strcat(death_string, " starved to death");
 break;

 } // end switch

if (death_type != 11 && death_type != 12)
{
 if (where_are_you == 1)
 {
   strcat(death_string, " in Dis.");
   goto ending;
 }
 if (where_are_you == 2)
 {
   strcat(death_string, " in Hell.");
   goto ending;
 }

        itoa((your_level + 1), st_prn, 10);

        strcat(death_string, " on level ");
        strcat(death_string, st_prn);
        strcat(death_string, ".");
//        if (death_type == 1) //

        ending : end_game(1);
}

 end_game(0); // must have won! (or at least escaped, but how could someone be that stupid?)

        //hp = -1000;


}




int which_cloud(char cx, char cy)
{
        int cc;
        for (cc = 0; cc < CLOUDS; cc++)
        {
                if (cloud_x [cc] == cx && cloud_y [cc] == cy)
                        return cc;
        }

return 200;

}



void in_a_cloud(void)
{

int cl = which_cloud(x_pos, y_pos);

switch(cloud_type [cl] % 100)
{
        case 1:
        strcpy(info [info_lines], "You are engulfed in roaring flames");
        if (res_fire == 0)
        {
                strcat(info [info_lines], "!");
                incr();
                ouch((random2(5) + random2(5) + random2(5) + 3) * (float) (time_taken / 10), cl, 2);
        } else
        {
                strcat(info [info_lines], ", but you don't feel hot!");
                incr();
        }
 scrolls_burn(7, 6);
        break;

   case 2:
   // If you don't have to breathe, unaffected
   strcpy(info [info_lines], "You are engulfed in noxious fumes!");
   incr();
   if (res_poison != 0) break;
   ouch(random2(3) * (float) (time_taken / 10), cl, 2);
   if (random2(5) + 1 >= xl) conf += random2(3) + 2;
   break;

        case 3:
        strcpy(info [info_lines], "You are engulfed in freezing vapours");
        if (res_cold == 0)
        {
                strcat(info [info_lines], "!");
                incr();
                ouch((random2(5) + random2(5) + random2(5) + 3) * (float) (time_taken / 10), cl, 2);
        } else
        {
                strcat(info [info_lines], ", but you don't feel cold!");
                incr();
        }
 scrolls_burn(7, 8);
        break;

   case 4: // poison
   // If you don't have to breathe, unaffected
   strcpy(info [info_lines], "You are engulfed in poison gas!");
   incr();
   if (res_poison != 0) break;
   ouch(random2(3) * (float) (time_taken / 10), cl, 2);
   poison ++;
   break;

   case 5:
                        case 6:
   case 7:
   strcpy(info [info_lines], "You are engulfed in a cloud of smoke!");
                        incr();
                        break;

   case 8:
   strcpy(info [info_lines], "You are engulfed in a cloud of scalding steam!");
   incr();
   ouch(random2(6) * (float) (time_taken / 10), cl, 2);
   break;

// case 5 // grey smoke - harmless
// case 6 blue smoke
// case 7 purple smoke

}

}







void end_game(char end_status)
{





/*      char del_file [25];

        int sysg = 0;

        strcpy(del_file, "del ");
        strcat(del_file, your_name);
        strcat(del_file, ".sav"); // .sav
        sysg = system(del_file);

        if (sysg == -1)
        {
                perror("Oh dear... ");
                exit(-1);
        }

*/





if (end_status == 1)
{
        strcpy(info [info_lines], "You die...");
        incr();
}

//      more();
        //if (info_lines > 0)
//      relay_message();
        brek = 1;
        viewwindow();
        more();

        char tm [10] [30];
        int handle;

        /*
        handle = open("control.dat", S_IWRITE, S_IREAD);

        for (i = 0; i < 10; i++)
        {
                read(handle, tm [i] , 30);
        }

        strcpy(tm [newc], "deceased");

        close (handle);




        handle = open("control.dat", O_WRONLY, S_IWRITE, S_IREAD);

        for (i = 0; i < 10; i++)
        {
                write(handle, tm [i] , 30);
        }

        //strcpy(tm [i], "deceased");

        close (handle);
        */



for (i = 0; i < 52; i ++)
{
 inv_ident [i] = 3;
}

for (i = 0; i < 52; i ++)
{
        if (inv_class [i] != 0)
        {
                set_id(inv_class [i], inv_type [i], 1);
        }
}

invent(-1, inv_quant, inv_dam, inv_class, inv_type, inv_plus, inv_ident, item_wielded, armour [0], armour [5], armour [2], armour [1], armour [3], armour [4], ring);



//      int sysg = 0;


/*
//      sysg = system("cd c:\\borlandc\\crawl\\levels");

        if (sysg == -1)
        {
                perror("Oh dear... ");
                exit(-1);
        }
*/





        //more();

for (p = 0; p < 52; p ++)
{
for (i = 0; i < ITEMS; i++)
{
        if (item_quant [i] == 0)
        {
//              strcpy(fake_name [i], inv_name [item_drop_2]);
                item_ident [i] = 0;//inv_ident [p];
                item_class [i] = inv_class [p];
                item_type [i] = inv_type [p];
                item_plus [i] = inv_plus [p];
                item_dam [i] = inv_dam [p];
                icolour [i] = inv_col [p];
                item_x [i] = x_pos;
                item_y [i] = y_pos; //x place position
                item_quant [i] = inv_quant [p];
                it_no ++;
                break;
        }
} // end of i loop
} // end of p loop


        item_cull(mons_class, mons_inv, item_class, item_type, item_dam, icolour, item_plus, item_quant, curr_level, grid, item_x, item_y, it_no, mons_x, mons_y);

        for (i = 0; i < ITEMS; i ++)
        {
                item_ident [i] = 0;
        }


        //save_level();


        clrscr();
        window(1,1,80,25);
        clrscr();
        cprintf("Goodbye, ");
        cprintf(your_name);
        cprintf(".");
        cprintf("\n\r\n\r");
        cprintf(death_string);
        cprintf("\n\r\n\r High Score List - \n\r");


highscore(death_string, points);



/*
        switch(end_status)
        {
        case 0: cprintf("When you quit, your stats were:\n\r"); break;
        case 1:
        switch(random2(3))
        {
        case 0: cprintf("When you shuffled off this mortal coil, your stats were:\n\r"); break;
        case 1: cprintf("When you sadly passed away, your stats were:\n\r"); break;
        case 2: cprintf("At the time of your death, your stats were:\n\r"); break;
//        case 3: cprintf("
        } // end of switch(random2)

        } // end of switch(end_status)
*/
/*      cprintf("When you ");
        switch(end_status)
        {
        case 0: cprintf("quit, "); break;
        case 1: cprintf("died, "); break;
        }

        cprintf("you were a level "); break;
*/
/*
        sysg = 0;

        sysg = system("cd c:\\borlandc\\crawl");

        if (sysg == -1)
        {
                perror("Oh dear... ");
                exit(-1);
        }
*/

//      keyin = getch();
        exit(0);
}




void splash_with_acid(char acid_strength)
{

// affects armour only?

// don't use beam variables - eg acid bolt whatever

// beam_damage is strength * amount of acid

// assume that a message has already been sent, also that damage has
// already been done


//abort();

char splc = 0;

for (splc = 0; splc < 6; splc++)
{
        if (armour [splc] == -1)
 {
        ouch(random2(acid_strength) / 5, 0, 3);

    continue;
    //should take extra damage if little armour worn
 }

        if (random2(20) > acid_strength) continue;

        item_corrode(armour [splc]);

}

}


void weapon_acid(char acid_strength)
{
        char hand_thing = item_wielded;

        if (item_wielded == -1)
        {
                if (armour [3] != -1) hand_thing = armour [3];
                        else return; // take extra damage
        }

        if (random2(20) > acid_strength) return;

        item_corrode(hand_thing);

}



void item_corrode(char itco)
{

        int chance_corr = 0;

        if (inv_plus [itco] < 45 | (inv_plus [itco] > 130 && inv_plus [itco] < 145)) return;
 if (inv_dam [itco] > 180) return; // unique

        chance_corr = inv_plus [itco];
        if (chance_corr > 130) chance_corr -= 100;

        if (chance_corr < 50)
        {
                chance_corr = 50 + ((chance_corr - 50) * -1);
        }

        chance_corr -= 50;

        //cout << chance_corr;
        //delay(800);
        //getch();

        switch(chance_corr)
        {
                case 0: if (random2(100) < 10) return; break;
                case 1: if (random2(100) < 25) return; break;
                case 2: if (random2(100) < 40) return; break;
                case 3: if (random2(100) < 70) return; break;
                case 4: if (random2(100) < 95) return; break;
                default: return;
        }

        item_name(inv_class [itco], inv_type [itco], inv_dam [itco], inv_plus [itco], inv_quant [itco], inv_ident [itco], 4, str_pass);

        strcpy(info [info_lines], str_pass);

        strcat(info [info_lines], " is eaten away!");
        incr();

        inv_plus [itco] --;
        if (inv_class [itco] == 2 && inv_type [itco] != 8 && inv_type [itco] != 13 && inv_type [itco] != 14) AC--;
 if (inv_class [itco] == 2 && (inv_type [itco] == 8 | inv_type [itco] == 13 | inv_type [itco] == 14)) shield_class --;
        AC_ch = 1;

}



void scrolls_burn(char burn_strength, char target_class)
{

unsigned char burnc;
unsigned char burn2;
unsigned char burn_no = 0;

for (burnc = 0; burnc < 52; burnc++)
{
        if (inv_quant [burnc] == 0) continue;
        if (inv_class [burnc] != target_class) continue; // 6 scrolls, 8 potions

        for (burn2 = 0; burn2 < inv_quant [burnc]; burn2++)
        {
                if (random2(20) < burn_strength)
                {
                        inv_quant [burnc] --;
                        burn_no++;
                        if (inv_quant [burnc] <= 0)
                        {
                                inv_no--;
                                if (burnc == item_wielded) // I can't assume any level of intelligence on the player's behalf.
                                {
                                        item_wielded = -1;
                                        strcpy(info [info_lines], "You are now empty handed.");
                                        incr();
                                }
                                break;
                        }
                }
        }
}

if (burn_no == 1)
{
        if (target_class == 6) strcpy(info [info_lines], "A scroll you are carrying catches fire!");
        if (target_class == 8) strcpy(info [info_lines], "A potion you are carrying freezes and shatters!");
 if (target_class == 4) strcpy(info [info_lines], "Some of your food is covered with spores!");
 incr();
}

if (burn_no > 1)
{
        if (target_class == 6) strcpy(info [info_lines], "Some of the scrolls you are carrying catch fire!");
        if (target_class == 8) strcpy(info [info_lines], "Some of the potions you are carrying freeze and shatter!");
 if (target_class == 4) strcpy(info [info_lines], "Some of your food is covered with spores!");
 incr();
}
// burn_no could be 0

}











void monster_polymorph(unsigned char mons, unsigned char targetc, int power)
{


if (targetc == 250)
{
   do
   {
      targetc = random2(52); // I think. Shouldn't poly into eg orc _wizard_
   } while (targetc == 31);
   //while (targetc == 23 | targetc == 49 | targetc == 31);
   // no //xX or F
}

//targetc = 25;

//if (power != -1) // automatic success

unsigned char old_class = mons_class [mons];
unsigned char old_hp = mons_hp [mons];
unsigned char old_hp_max = mons_hp_max [mons];
unsigned char old_sec = mons_sec [mons];

// deal with mons_sec


mons_class [mons] = targetc;


/*
if (targetc == 25 | targetc == 51) // zZ
{
   do
   {
      mons_sec [mons] = random2(52);
   } while (mons_zombie_size((int) mons_sec [mons]) == 0);

   if (mons_zombie_size((int) mons_sec [mons]) == 1)
   {
      mons_class [mons] = 25;
   } else mons_class [mons] = 51;
}*/

//mons_sec [mons] = 1;


//int mons_zombie_size(int mclass)

mons_sec [mons] = 250;

k = mons;

get_monster();


mons_hp [mons] = mons_hp_max [mons] * (float) ((old_hp * 100) / old_hp_max) / 100;
mons_speed_inc [mons] = 70 + random2(5);

// make sure that eg manticores don't drop their darts

monster_drop_ething(mons);

int unenc = 0;

for (unenc = 0; unenc < 3; unenc++)
{
   mons_ench [mons] [unenc] = 0;
}

mons_ench_1 [mons] = 0;
// need to tweak this for invis

strcpy(info [info_lines], monam (old_sec, old_class, mons_ench [mons] [2], 0));
strcat(info [info_lines], " evaporates, and reforms as ");
//incr();

//strcpy(info [info_lines], "It reforms in the shape of ");
strcat(info [info_lines], monam (mons_sec [mons], mons_class [mons], mons_ench [mons] [2], 3));
strcat(info [info_lines], "!");
incr();

} // end of monster_polymorph


void monster_drop_ething(int monster_killed)
{
// use monster_killed for convenience
// drop weapons & missiles last (ie on top) to pick up if pmorphed

int ygy;

for (ygy = 7; ygy >= 0; ygy --)
        {
        if (mons_inv [monster_killed] [ygy] != 501)
        {
  item_link [mons_inv [monster_killed] [ygy]] = 501;
                if (igrid [mons_x [monster_killed]] [mons_y [monster_killed]] == 501)
                // this bit is wrong, BTW:
                //if (mons_inv [monster_killed] [j] != 501)
                {
                        igrid [mons_x [monster_killed]] [mons_y [monster_killed]] = mons_inv [monster_killed] [ygy];
                } else
                {
                        int huj = igrid [mons_x [monster_killed]] [mons_y [monster_killed]];
                        igrid [mons_x [monster_killed]] [mons_y [monster_killed]] = mons_inv [monster_killed] [ygy];
                        item_link [mons_inv [monster_killed] [ygy]] = huj;
                }
        }


        mons_inv [monster_killed] [ygy] = 501;
        //itc = 0;
        }



} // end of void monster_drop_ething


void alert(void)
{
// if you're invis and throw/zap whatever, alerts mons to your position

int it;

for (it = 0; it < MNST; it++)
{

if (mons_class [it] == -1) continue;

if (mons_near(it) == 1)
{
   if (mons_beh [it] == 1 | mons_beh [it] == 2 | mons_beh [it] == 0)
   {
      mons_beh [it] = 1;
      mons_targ_1_x [it] = x_pos;
      mons_targ_1_y [it] = y_pos;
   }
}

} // end of for it

} // end of void alert()



int jelly_divide(int jel)
{

         if (mons_hp [jel] == 1) return 0;

         int jex = 0;
         int jey = 0;

         for (jex = -1; jex < 3; jex ++)
         {

            if (jex == 2) return 0;

            for (jey = -1; jey < 2; jey ++)
            {
               if (grid [mons_x [jel] + jex] [mons_y [jel] + jey] < 50 && grid [mons_x [jel] + jex] [mons_y [jel] + jey] > 10 && (mons_x [jel] + jex != x_pos | mons_y [jel] + jey != y_pos))
               // 10-50 for now - must take clouds into account
               goto jel2;
            } // end of for jey

         } // end of for jex

         jel2: for (k = 0; k < MNST; k++)
         {
               if (k == MNST - 1) return 0;

               if (mons_class [k] == -1) break;
         }

         mons_hp_max [jel] /= 2;
         if (mons_hp [jel] > mons_hp_max [jel]) mons_hp [jel] = mons_hp_max [jel];

mons_class [k] = 35;
mons_HD [k] = 4;
mons_hp [k] = mons_hp [jel];   //(random2((mons_HD [k]) * 10) + (mons_HD [k]) * 10);
mons_hp_max [k] = mons_hp [k];
mons_AC [k] = 0;//gmon_AC [2];
mons_ev [k] = 2;//gmon_ev [2];
mons_speed [k] = 5;
mons_speed_inc [k] = 70 + random2(5);

mons_x [k] = mons_x [jel] + jex;
mons_y [k] = mons_y [jel] + jey;

grid [mons_x [k]] [mons_y [k]] += 50;

         no_mons ++;
         mn_no ++;

         mons_beh [k] = 0; // Look at this!

         if (mons_near(jel) == 1)
         {
            strcpy(info [info_lines], "The jelly splits in two!");
            incr();
         } else
           {
               strcpy(info [info_lines], "You hear a squelching noise in the distance.");
               incr();
           }

         return 1;

}
















char monsters_fight(int monster_attacking, int monster_attacked)
{
//float mons_to_hit;
damage_taken = 0;
char hit = 0;

char use_weap = 0;

int specdam = 0;

hand_used = 0;

killer = 2;

int sees = 0;

if (mons_class [monster_attacking] == 32)
{
                mons_hp [monster_attacking] = -1;
                return;
}

if (mons_class [monster_attacking] >= MLAVA0 && mons_sec [monster_attacking] == 1) return;

if (mons_beh [monster_attacking] == 7)
{
        if (mons_beh [monster_attacked] == 7)   return 0;
        mons_hit [monster_attacking] = monster_attacked;
}

if (mons_beh [monster_attacked] == 6)
{
        if (mons_beh [monster_attacked] != 7 && monster_attacked != mons_hit [monster_attacking])
                        return 0;
}

//int runthru = 0;

//mons_to_hit = mons_HD [monster_attacking]; // not finished yet.

if (mons_near(monster_attacking) == 1 && mons_near(monster_attacked) == 1) sees = 1;

for (runthru = 0; runthru < 4; runthru ++)
{

char mdam = mondamage(mons_class [monster_attacking], runthru);

if (mons_class [monster_attacking] == 25 | mons_class [monster_attacking] == 51)
{
        mdam = mondamage(mons_sec [monster_attacking], runthru);
        if (mdam > 1 && mdam < 4)
        {
                mdam --;
        }

        if (mdam > 3 && mdam < 8)
        {
                mdam -= 2;
        }

        if (mdam > 7 && mdam < 13)
        {
                mdam -= 3;
        }

        if (mdam > 12)
        {
                mdam -= 5;
        }
}


if (mdam == 0) break;
//gmon_att [mons_class [monster_attacking]] [runthru] == 0) break;

if (mons_class [monster_attacking] == 30 && runthru == 1 && mons_inv [monster_attacking] [1] != 501)
{
        hand_used = 1;
}

damage_taken = 0;


mons_to_hit = 20 + mons_HD [monster_attacking]; // * mons_HD [monster_attacking]; // * 3  //strength + (0.5 * dex) + (f_abil);

if (mons_inv [monster_attacking] [hand_used] != 501)
{
        if (item_plus [mons_inv [monster_attacking] [hand_used]] - 50 > 130)
        {
                mons_to_hit += random2(item_plus [mons_inv [monster_attacking] [hand_used]] - 50 - 100 + 1);
        } else
              {
                mons_to_hit += random2(item_plus [mons_inv [monster_attacking] [hand_used]] - 50 + 1); /// 10;
              }

//      mons_to_hit += item_plus [mons_inv [monster_attacking] [0]];
        mons_to_hit += 3 * property [0] [item_type [mons_inv [monster_attacking] [0]]] [1];

   mons_speed_inc [monster_attacking] -= (float) ((float) (property [inv_class [item_wielded]] [inv_type [item_wielded]] [2] - 10)); // / 10);

}

//if (inv_class [item_wielded] == 0) mons_to_hit += inv_plus [item_wielded];


//mons_to_hit *= random2(200) / 100;

mons_to_hit = random2(mons_to_hit);

/*
//if (mons_to_hit >= (evasion - 6)) //&& mons_to_hit < evasion && shield_armour != -1) // && random2(6) != 0)
if (mons_to_hit < evasion)
{
strcpy(info [info_lines], "You block the ");
strcat(info [info_lines], gmon_name [mons_class [monster_attacking]]);
if (mons_inv [monster_attacking] [0] != 255)
        {
        strcat(info [info_lines], "'s ");
        strcat(info [info_lines], fake_name [mmov_x]);
        }
strcat(info [info_lines], " on your shield!");
incr();
return;
}
*/


        // This bit is a bit clumsy:
   //      not any more.
        if (mons_class [monster_attacking] == 10 | mons_class [monster_attacking] == 22)
        {
/*              if (haste == 0) mons_speed_inc [monster_attacking] -= (mons_speed [monster_attacking] * time_taken);
                        else mons_speed_inc [monster_attacking] -= (mons_speed [monster_attacking] * (time_taken / 2));

                if (slow > 0) mons_speed_inc [monster_attacking] -= (mons_speed [monster_attacking] * time_taken);*/

                // maybe this will work better:
                mons_speed_inc [monster_attacking] -= 10; //--
        }






if (mons_to_hit >= mons_ev [monster_attacked])// | random2(6) == 0) // mons_ev [monster_attacked])
//if (random2(20) + mons_to_hit >= 20 - (10 - AC)) // hit roll
{
hit = 1;

//te = mons_inv [monster_attacking] [0];

//k = item_class [te];
//p = item_type [te];
//j = property [k] [p] [0];
//o = mons_class [monster_attacking];
//k = gmon_att [o] [runthru];


if (mons_inv [monster_attacking] [hand_used] != 501 && item_class [mons_inv [monster_attacking] [hand_used]] == 0 && item_type [mons_inv [monster_attacking] [hand_used]] < 13 | item_type [mons_inv [monster_attacking] [hand_used]] > 15)
{
//damage_taken = random2(mmov_x);

damage_taken = random2(property [item_class [mons_inv [monster_attacking] [0]]] [item_type [mons_inv [monster_attacking] [0]]] [0]);
//if (item_plus [mons_inv [i] [0]] > 80) damage_taken -= 100;
//damage_taken += item_plus [mons_inv [i] [0]];
        if (item_plus [mons_inv [monster_attacking] [hand_used]] > 130)
        {
                damage_taken += random2(item_plus [mons_inv [monster_attacking] [hand_used]] - 150 + 1);
        } else
              {
                damage_taken += random2(item_plus [mons_inv [monster_attacking] [hand_used]] - 50 + 1);// % 10;
              }
damage_taken -= random2(3) + 1;//1;
}

damage_taken += random2(mdam) + 1;
//gmon_att [mons_class [monster_attacking]] [runthru]) + 1;
//damage_taken /= (random2 (AC) + 1); // / 3
//damage_taken *= 2;

//if (mons_inv [monster_attacking] [0] != 501)
//{
//      damage_taken /= ((random2 (AC) / property [item_class [mons_inv [i] [0]]] [item_type [mons_inv [i] [0]]] [2]) + 1);
//} else
//      damage_taken /= (random2 (AC) + 1);

        damage_taken -= random2(mons_AC [monster_attacked] + 1);

if (damage_taken < 1) damage_taken = 0;
//hp -= (int) damage_taken;
//ouch(damage_taken, monster_attacking, 0);

//hp_ch = 1;

} // end of if mons_to_hit >= evasion
 else
 {
        hit = 0;
//      strcpy(info [info_lines], "The ");
        //} else
        //strcat(info [info_lines], "The ");

        strcpy(info [info_lines], monam (mons_sec [monster_attacking], mons_class [monster_attacking], mons_ench [monster_attacking] [2], 0));  //gmon_name [mons_class [monster_attacking]]);
        strcat(info [info_lines], " misses ");
        strcat(info [info_lines], monam (mons_sec [monster_attacked], mons_class [monster_attacked], mons_ench [monster_attacked] [2], 1));
   strcat(info [info_lines], ".");
   if (sees == 1) incr();
 }


if (damage_taken < 1 && hit == 1)
{
//if (78 - strlen(info [info_lines]) < strlen(gmon_name [mons_class [monster_attacking]]) + 17)
//      {
        //incr();
//      strcpy(info [info_lines], "The ");
        //} else
        //strcat(info [info_lines], "The ");


        strcpy(info [info_lines], monam (mons_sec [monster_attacking], mons_class [monster_attacking], mons_ench [monster_attacking] [2], 0));  //gmon_name [mons_class [monster_attacking]]);
        strcat(info [info_lines], " hits ");
        strcat(info [info_lines], monam (mons_sec [monster_attacked], mons_class [monster_attacked], mons_ench [monster_attacked] [2], 1));
        strcat(info [info_lines], ".");// but doesn't do any damage.");
 if (sees == 1) incr();
}



if (hit == 1) //(int) damage_taken >= 1)
{
//if (78 - strlen(info [info_lines]) < strlen(gmon_name [mons_class [monster_attacking]]) + 15)
//      {
//      incr();
//      strcpy(info [info_lines], "The ");
//      } else
//      strcpy(info [info_lines], "The ");

        mmov_x = mons_inv [monster_attacking] [hand_used];

        strcpy(info [info_lines], monam (mons_sec [monster_attacking], mons_class [monster_attacking], mons_ench [monster_attacking] [2], 0));
        strcat(info [info_lines], " hits ");
        strcat(info [info_lines], monam (mons_sec [monster_attacked], mons_class [monster_attacked], mons_ench [monster_attacked] [2], 1));


/*        strcat(info [info_lines], " for ");
        itoa(damage_taken, st_prn, 10);
        strcat(info [info_lines], st_prn);
*/


//      itoa(

        if (mons_inv [monster_attacking] [hand_used] != 501 && item_class [mons_inv [monster_attacking] [hand_used]] == 0 && item_type [mons_inv [monster_attacking] [hand_used]] < 13 | item_type [mons_inv [monster_attacking] [hand_used]] > 15)
        {
        strcat(info [info_lines], " with ");

        item_name(item_class [mmov_x], item_type [mmov_x], item_dam [mmov_x], item_plus [mmov_x], item_quant [mmov_x], item_ident [mmov_x], 7, str_pass);
        strcat(info [info_lines], str_pass);
        }
        strcat(info [info_lines], "! "); // put something for '.', '!', '!!' etc depending on what % of damage you took.

 if (sees == 1) incr();

        // special attacks:

        brek = 0;



        switch(mons_class [monster_attacking])
        {
        case 0: // giant ant
        if ((damage_taken >= 4 && random2(4) == 0) | random2(20) == 0)
        {
        strcpy(info [info_lines], "The giant ant stings ");
      strcat(info [info_lines], monam (mons_sec [monster_attacked], mons_class [monster_attacked], mons_ench [monster_attacked] [2], 1));
      strcat(info [info_lines], ".");
      if (sees == 1) incr();
      //        poison++;
      poison_monster(monster_attacked, 1);
   }
        break;

        case 2: // cockatrice.
        break;

        case 10: // killer bee
        if (((damage_taken >= 3 && random2(3) == 0) | random2(20) == 0))
        {
     strcpy(info [info_lines], "The killer bee stings ");
     strcat(info [info_lines], monam (mons_sec [monster_attacked], mons_class [monster_attacked], mons_ench [monster_attacked] [2], 1));
     strcat(info [info_lines], ".");
     if (sees == 1) incr();
     // poison += 1;
     poison_monster(monster_attacked, 1);
   }
        break;

        case 13: // necrophage
        if ((damage_taken >= 3 && random2(3) == 0) | random2(20) == 0)
        {
                //strcpy(info [info_lines], "You feel your flesh start to rot away!");
                //incr();
                //rotting += random2(3) + 1;
      mons_hp_max [monster_attacked] -= random2(3) + 1;
      if (mons_hp [monster_attacked] > mons_hp_max [monster_attacked]) mons_hp [monster_attacked] = mons_hp_max [monster_attacked];
        }
        break;

 case 21: // fire_vortex
 mons_hp [monster_attacking] = -10;
   specdam = 0;
   if (mons_res_fire(mons_class [monster_attacked]) == 0 | (mons_inv [monster_attacked] [2] == 501 | item_dam [mons_inv [monster_attacked] [2]] % 30 != 2))
   {
    specdam = random2(15) + 15;
   }
   if (mons_res_fire(mons_class [monster_attacked]) == -1 | (mons_inv [monster_attacked] [2] == 501 | item_dam [mons_inv [monster_attacked] [2]] % 30 != 2))
   {
    specdam = random2(25) + 20;
   }
    if (specdam != 0)
    {
      if (sees == 1)
      {
        strcpy(info [info_lines], monam (mons_sec [monster_attacked], mons_class [monster_attacked], mons_ench [monster_attacked] [2], 0));
        strcat(info [info_lines], " is engulfed in flame!");
        //if (sees == 1)
                                                                incr();
      }
    }
   damage_taken += specdam;
 break;

        case 42: // killer bee
//      if ((damage_taken >= 3 && random2(3) == 0) | random2(20) == 0)
//      {
        strcpy(info [info_lines], "The queen bee stings ");
   strcat(info [info_lines], monam (mons_sec [monster_attacked], mons_class [monster_attacked], mons_ench [monster_attacked] [2], 1));
   strcat(info [info_lines], ".");
   if (sees == 1) incr();
   poison_monster(monster_attacked, 1);
//      poison += 2;
//      }
        break;


        case 18: // snake
        if ((damage_taken >= 3 && random2(4) == 0) | random2(20) == 0)
        {
        strcpy(info [info_lines], "The snake's bite poisons ");
   strcat(info [info_lines], monam (mons_sec [monster_attacked], mons_class [monster_attacked], mons_ench [monster_attacked] [2], 1));
   strcat(info [info_lines], ".");
   if (sees == 1) incr();
        poison_monster(monster_attacked, 1);
        }
        break;

 case 60: // wight
        case 48: // wraith
        if ((damage_taken >= 6 && random2(4) == 0) | random2(30) == 0)
        {
      strcpy(info [info_lines], monam (mons_sec [monster_attacked], mons_class [monster_attacked], mons_ench [monster_attacked] [2], 0));
      strcat(info [info_lines], " is drained.");
      if (sees == 1) incr();
      mons_HD [monster_attacked] --;
      mons_hp_max [monster_attacked] -= 3 + random2(6);
      mons_hp [monster_attacked] -= 3 + random2(6);
      if (mons_hp [monster_attacked] >= mons_hp_max [monster_attacked]) mons_hp [monster_attacked] = mons_hp_max [monster_attacked];
      if (mons_hp [monster_attacked] <= 0 | mons_HD [monster_attacked] <= 0)
      {
            monster_die(monster_attacked);
            return 1;
      }
      //brek = 1;
        }
        break;

        case 22: // giant wasp
/*      if ((damage_taken >= 3 && random2(3) == 0) | random2(20) == 0)
        {
        if (paralysis > 0)
        {
                strcpy(info [info_lines], "You still can't move!");
        } else strcpy(info [info_lines], "You suddenly lose the ability to move!");
        incr();
        paralysis += random2(3) + 1;
        }*/
        break;

        case 35: // Jelly
//      strcpy(info [info_lines], "You are splashed with acid!");
//      incr();
//      splash_with_acid(10);
        break;




 case 34: // Ice beast
   specdam = 0;
   if (mons_res_cold(mons_class [monster_attacked]) == 0 && (mons_inv [monster_attacked] [2] == 501 | item_dam [mons_inv [monster_attacked] [2]] % 30 != 3))
   {
    specdam = random2(10) + 5;
   }
   if (mons_res_cold(mons_class [monster_attacked]) == -1 && (mons_inv [monster_attacked] [2] == 501 | item_dam [mons_inv [monster_attacked] [2]] % 30 != 3))
   {
    specdam = random2(15) + 10;
   }
    if (specdam != 0)
    {
      if (sees == 1)
      {
        strcpy(info [info_lines], monam (mons_sec [monster_attacking], mons_class [monster_attacking], mons_ench [monster_attacking] [2], 0));
        strcat(info [info_lines], " freezes ");
        strcat(info [info_lines], monam (mons_sec [monster_attacked], mons_class [monster_attacked], mons_ench [monster_attacked] [2], 1));
        if (sees == 1) incr();
      }
    }
   damage_taken += specdam;
 break;

 case 47: // Vampire
 if (damage_taken >= 7 && random2(2) == 0 | random2(10) == 0)
 mons_hp_max [monster_attacked] -= 3;
 if (mons_hp [monster_attacked] >= mons_hp_max [monster_attacked]) mons_hp [monster_attacked] = mons_hp_max [monster_attacked];
 break;




        case 39: // Naga

        break;


        } // end of switch for special attacks.

        // use brek for level drain, maybe with beam variables, because so many creatures use it.

//if (brek == 1) // energy drain:
//{
//lose_level();

       /*
        xp = (xp - 20 * (xl - 1) * (xl - 1) - 20 * (xl - 2) * (xl - 2));

        xl --;

//      if (xp > (20 * xl * xl) && xl < 27)
//      {
                //xl++;
                strcpy(info [info_lines], "You are now a level ");
                itoa(xl, temp_quant, 10);
                strcat(info [info_lines], temp_quant);
                strcat(info [info_lines], " ");
                strcat(info [info_lines], clasnam);
                strcat(info [info_lines], "!");
                incr();
//              message();
//              more();
        switch(clas)
        {
        case 0: // fighter
        brek = random2(6) + 5;
        hp -= brek;
        hp_max -= brek;
        brek = random2(3) + 1;
        ep -= brek;
        ep_max -= brek;
        f_abil -= 1;
        speed += .02;
        break;

        }

        rate_regen /= 1.1;

        xl_ch = 1; hp_ch = 1; ep_ch = 1; xp_ch = 1;

        brek = 0;
*/

//}

}

// special_weapons

if (mons_inv [monster_attacking] [hand_used] != 501 && hit == 1)
{
 switch(item_dam [mons_inv [monster_attacking] [hand_used]])
  {
   case 0: // nothing
   break;

   case 1: // flaming
   specdam = 0;
   if (mons_res_fire(mons_class [monster_attacked]) == 0 && (mons_inv [monster_attacked] [2] == 501 | item_dam [mons_inv [monster_attacked] [2]] % 30 != 2))
   {
    specdam = random2(damage_taken) / 2 + 1;
   }
   if (mons_res_fire(mons_class [monster_attacked]) == -1 && (mons_inv [monster_attacked] [2] == 501 | item_dam [mons_inv [monster_attacked] [2]] % 30 != 2))
   {
    specdam = random2(damage_taken) + 1;
   }
    if (specdam != 0)
    {
      if (sees == 1)
      {
        strcpy(info [info_lines], monam (mons_sec [monster_attacking], mons_class [monster_attacking], mons_ench [monster_attacking] [2], 0));
        strcat(info [info_lines], " burns ");
        strcat(info [info_lines], monam (mons_sec [monster_attacked], mons_class [monster_attacked], mons_ench [monster_attacked] [2], 1));
        if (specdam < 3) strcat(info [info_lines], ".");
        if (specdam >= 3 && specdam < 7) strcat(info [info_lines], "!");
        if (specdam >= 7) strcat(info [info_lines], "!!");
        //    strcat(info [info_lines], " is burned.");
        incr();
        damage_taken += specdam;
      }
    }
   damage_taken += specdam;
   break;

   case 2: // freezing
   specdam = 0;
   if (mons_res_cold(mons_class [monster_attacked]) == 0 && (mons_inv [monster_attacked] [2] == 501 | item_dam [mons_inv [monster_attacked] [2]] % 30 != 3))
   {
    specdam = random2(damage_taken) / 2 + 1;
   }
   if (mons_res_cold(mons_class [monster_attacked]) == -1 && (mons_inv [monster_attacked] [2] == 501 | item_dam [mons_inv [monster_attacked] [2]] % 30 != 3))
   {
    specdam = random2(damage_taken) + 1;
   }
    if (specdam != 0)
    {
      if (sees == 1)
      {
        strcpy(info [info_lines], monam (mons_sec [monster_attacking], mons_class [monster_attacking], mons_ench [monster_attacking] [2], 0));
        strcat(info [info_lines], " freezes ");
        strcat(info [info_lines], monam (mons_sec [monster_attacked], mons_class [monster_attacked], mons_ench [monster_attacked] [2], 1));
        if (specdam < 3) strcat(info [info_lines], ".");
        if (specdam >= 3 && specdam < 7) strcat(info [info_lines], "!");
        if (specdam >= 7) strcat(info [info_lines], "!!");
        //    strcat(info [info_lines], " is burned.");
        incr();
        //damage_taken += specdam;
      }
    }
   damage_taken += specdam;
   break;

   case 3: // holy wrath
   specdam = 0;
   switch(mons_holiness(mons_class [monster_attacked]))
   {
    case -1:
//    strcpy(info [info_lines], "
      damage_taken -= 5 + random2(5);
    break;

    case 1:
      specdam += random2(damage_taken) + 1;
    break;

    case 2:
      specdam += random2(damage_taken) * 1.5 + 1; // does * 1.5 do anything?
    break;
   }
//   if (specdam > 0)
//   {
//    strcpy(info [info_lines], "
    damage_taken += specdam;
//   }
   break;

   case 4: // electrocution
//   if (lev != 0) break; // you're not grounded
   if (mons_flies(mons_class [monster_attacked]) == 2) break;
   specdam = 0;

   if (item_plus [mons_inv [monster_attacking] [hand_used]] <= 50 | item_plus [mons_inv [monster_attacking] [hand_used]] > 130 && item_plus [mons_inv [monster_attacking] [hand_used]] <= 150) break;
   if (random2(3) == 0)
   {
    strcpy(info [info_lines], "There is a sudden explosion of sparks!");
    if (sees == 1) incr();
    specdam += 10 + random2(15);
    item_plus [mons_inv [monster_attacking] [hand_used]] --;
   }
   damage_taken += specdam;
   break;

   case 5: // orc slaying
   if (mons_class [monster_attacked] == 14 | mons_class [monster_attacked] == 52 | mons_class [monster_attacked] == 54 | mons_class [monster_attacked] == 55)
   {
     mons_hp [monster_attacked] -= 1 + random2(damage_taken);
   }
   break;

   case 6: // venom
   if (random2(3) == 0) poison_monster(monster_attacked, 0);
   break;

//   case 7: // protection
 case 8:
        if (mons_holiness(mons_class [monster_attacked]) > 0 && ((damage_taken >= 6 && random2(4) == 0) | random2(30) == 0))
        {
      strcpy(info [info_lines], monam (mons_sec [monster_attacked], mons_class [monster_attacked], mons_ench [monster_attacked] [2], 0));
      strcat(info [info_lines], " is drained.");
      if (sees == 1) incr();
      mons_HD [monster_attacked] --;
      mons_hp_max [monster_attacked] -= 3 + random2(6);
      mons_hp [monster_attacked] -= 3 + random2(6);
      if (mons_hp [monster_attacked] >= mons_hp_max [monster_attacked]) mons_hp [monster_attacked] = mons_hp_max [monster_attacked];
      if (mons_hp [monster_attacked] <= 0 | mons_HD [monster_attacked] <= 0)
      {
            monster_die(monster_attacked);
            return 1;
      }
      //brek = 1;
        }
 break;

  }
} // end of if special weapon


//incr();

if (damage_taken > 0)
{
        //ouch(damage_taken, monster_attacking, 0);
        //hp_ch = 1;
   mons_hp [monster_attacked] -= damage_taken;
   if (mons_hp [monster_attacked] <= 0)
   {
      killer = 2;
      monster_die(monster_attacked);
      return 1;
   }
}


} // end of for runthru

if (mons_beh [monster_attacked] == 7)
{
//   mons_beh [monster_attacked] = 6;
                mons_hit [monster_attacked] = monster_attacking;
   mons_hit [monster_attacking] = monster_attacked;
//  }
}

if (mons_beh [monster_attacking] != 4)// && mons_beh [monster_attacking] != 7) // the latter is so that mons_beh [attacker] doesn't get reset to 6
{
  if (mons_beh [monster_attacked] <= 2 && distance(x_pos, mons_x [monster_attacked], y_pos, mons_y [monster_attacked]) > 1)
  {
    mons_beh [monster_attacked] = 6;
    mons_hit [monster_attacked] = monster_attacking; // This'll do for now.
  }
}

return 1;
} // end of void monsters_fight(int monster_attacking, ...)
















int mons_ench_f2(int o, char is_near, int func_pass [10])
{

char brek = 0;
char p;
//func_pass [0] = info_lines;

                switch(beam_colour) // put in magic resistance
                {
                case 0: // slow monster
                for (p = 0; p < 3; p ++)
                {
                        if (mons_ench [o] [p] == 1)
                        {
                                strcpy(info [info_lines], "Nothing appears to happen.");
                                incr();
                                return info_lines;
                        }
                        if (mons_ench [o] [p] == 2)
                        {
                                mons_ench [o] [p] = 0;
                                if (mons_speed [o] >= 100)
                                {
                                        mons_speed [o] = ((mons_speed [o] - 100) / 2) + 100;
                                } else mons_speed [o] /= 2;
                                if (mons_ench [o] [0] == 0 && mons_ench [o] [1] == 0 && mons_ench [o] [2] == 0)
                                        mons_ench_1 [o] = 0;
                                brek = 1;
                                break;
                        }
                } // end of for p

                if (brek == 0)
                        for (p = 0; p < 3; p ++)
                        {
                         if (mons_ench [o] [p] == 0) // | p == 2) // replaces 3rd enchantment if all full.
                                        {

                                                mons_ench [o] [p] = 1;
                                                if (mons_speed [o] >= 100)
                                                {
                                                         mons_speed [o] = ((mons_speed [o] - 100) / 2) + 100;
                                                } else mons_speed [o] /= 2;
                                                mons_ench_1 [o] = 1;
                                                break;
                     }
         }


              // put in an exception for fungi, plants and other things you won't notice slow down.
//              strcpy(info [info_lines], "The ");
                strcpy(info [info_lines], monam (mons_sec [o], mons_class [o], mons_ench [o] [2], 0));//monam (mons_class [o], mons_ench [o] [2], 0, see_invis)); //gmon_name [mons_class [o]]);
                strcat(info [info_lines], " seems to slow down.");
                //func_pass [0]++;
      incr();
                func_pass [1] = 1;
                return info_lines;// break;

                case 1: // haste
                for (p = 0; p < 3; p ++)
                {
                        if (mons_ench [o] [p] == 2) goto nothinghap; //return info_lines;
                        if (mons_ench [o] [p] == 1)
                        {
                                mons_ench [o] [p] = 0;
                                if (mons_speed [o] >= 100)
                                {
                                        mons_speed [o] = ((mons_speed [o] - 100) * 2) + 100;
                                } else mons_speed [o] *= 2;
                                if (mons_ench [o] [0] == 0 && mons_ench [o] [1] == 0 && mons_ench [o] [2] == 0)
                                        mons_ench_1 [o] = 0;
                                brek = 1;
                                break;
                        }
                } // end of for p

                if (brek == 0)
                        for (p = 0; p < 3; p ++)
                        {
                        if (mons_ench [o] [p] == 0) // | p == 2) // replaces 3rd enchantment
                                {
                                mons_ench [o] [p] = 2;
                                if (mons_speed [o] >= 100)
                                {
                                        mons_speed [o] = ((mons_speed [o] - 100) * 2) + 100;
                                } else mons_speed [o] *= 2;
                                mons_ench_1 [o] = 1;
                                break;
                                }
                        }

         if (p == 2 && mons_ench [o] [p] != 0) goto nothinghap;

                        /*
                if (mons_ench [o] == 2) return;
                if (mons_ench [o] == 1)
                {
                        mons_ench [o] = 0;
                        mons_speed [o] *= 2;
                } else
                {
                mons_ench [o] = 2;
                mons_speed [o] *= 2;
                }
                */
                // put in an exception for fungi, plants and other things you won't notice speed up.
//              strcpy(info [func_pass [0]], "The ");
                strcpy(info [info_lines], monam (mons_sec [o], mons_class [o], mons_ench [o] [2], 0));//monam (mons_class [o], mons_ench [o] [2], 0, see_invis)); //gmon_name [mons_class [o]]);
                strcat(info [info_lines], " seems to speed up.");
                //func_pass [0]++;
      incr();
                func_pass [1] = 1;
                return info_lines;// break;


                case 2: // healing
                if (mons_hp [o] == mons_hp_max [o])
                {
/*              strcpy(info [func_pass [0]], "Nothing appears to happen.");
                func_pass [0]++;
                return info_lines; //info_lines;*/
      goto nothinghap;
                }
                mons_hp [o] += random2(15) + 5;
                if (mons_hp [o] >= mons_hp_max [o])
                {
                mons_hp [o] = mons_hp_max [o];
//              strcpy(info [func_pass [0]], "The ");
                strcpy(info [info_lines], monam (mons_sec [o], mons_class [o], mons_ench [o] [2], 0));//monam (mons_class [o], mons_ench [o] [2], 0, see_invis)); //gmon_name [mons_class [o]]);
                strcat(info [info_lines], "'s wounds heal themselves!");
                incr();
                func_pass [1] = 1;
                return info_lines;
                }
//              strcpy(info [func_pass [0]], "The ");
                strcpy(info [info_lines], monam (mons_sec [o], mons_class [o], mons_ench [o] [2], 0));//monam (mons_class [o], mons_ench [o] [2], 0, see_invis)); //gmon_name [mons_class [o]]);
                strcat(info [info_lines], " is healed somewhat."); //is a little less battered.");
                incr();
                func_pass [1] = 1;
                return info_lines;

                case 3: // paralysis
  mons_speed_inc [o] = 0;
                if (is_near == 1)
                {
                        strcpy(info [info_lines], monam (mons_sec [o], mons_class [o], mons_ench [o] [2], 0));//monam (mons_class [o], mons_ench [o] [2], 0, see_invis)); //gmon_name [mons_class [o]]);
                        strcat(info [info_lines], " suddenly stops moving!");
                        incr();
                        func_pass [1] = 1;
                }
   if (grid [mons_x [o]] [mons_y [o]] % 50 == 11 | grid [mons_x [o]] [mons_y [o]] % 50 == 12)
                        {
        if (mons_flies(mons_class [o]) == 1)
                                {
                if (is_near == 1) // don't worry about invisibility - you should be able to see if something has fallen into the lava
                {
                        strcpy(info [info_lines], monam (mons_sec [o], mons_class [o], mons_ench [o] [2], 0));//monam (mons_class [o], mons_ench [o] [2], 0, see_invis)); //gmon_name [mons_class [o]]);
   if (grid [mons_x [o]] [mons_y [o]] % 50 == 12)
                        {
                        strcat(info [info_lines], " falls into the water!");
   } else strcat(info [info_lines], " falls into the lava!");
                           incr();
//                      func_pass [1] = 1;
                }
                switch(thing_thrown)
                {
                        case 1: killer = 1; break; // your beam
                        case 3: killer = 1; break; //  "    "
                        case 2: killer = 4; break; // dragon breath &c
                }
                                                monster_die(o);
                                }
                        }
  return info_lines;
        /*      for (p = 0; p < 3; p ++)
                {
                        if (mons_ench [o] [p] == 3) return info_lines;
                } // end of for p

                for (p = 0; p < 3; p ++)
                        {
                        if (mons_ench [o] [p] == 0 | p == 2) // replaces 3rd enchantment if all full.
                                {
                                mons_ench [o] [p] = 3;
                                mons_speed_inc [o] = 10;
                                mons_ench_1 [o] = 1;
                                break;
                                }
                        }
                // put in an exception for fungi, plants and other things you won't notice slow down.
//              strcpy(info [func_pass [0]], "The ");
                if (is_near == 1)
                {
                        strcpy(info [info_lines], monam (mons_sec [o], mons_class [o], mons_ench [o] [2], 0));//monam (mons_class [o], mons_ench [o] [2], 0, see_invis)); //gmon_name [mons_class [o]]);
                        strcat(info [info_lines], " suddenly stops moving!");
                        incr();
                        func_pass [1] = 1;
                }
                return info_lines;// break;

*/

                case 4: // confusion
                for (p = 0; p < 3; p ++)
                {
                        if (mons_ench [o] [p] == 4) return info_lines;
                } // end of for p

                for (p = 0; p < 3; p ++)
                        {
                        if (mons_ench [o] [p] == 0 | p == 2) // replaces 3rd enchantment if all full.
                                {
                                mons_ench [o] [p] = 5;
                                mons_ench_1 [o] = 1;
                                break;
                                }
                        }
                // put in an exception for fungi, plants and other things you won't notice slow down.
//              strcpy(info [func_pass [0]], "The ");
                if (is_near == 1)
                {
                        strcpy(info [info_lines], monam (mons_sec [o], mons_class [o], mons_ench [o] [2], 0)); //monam (mons_class [o], mons_ench [o] [2], 0, see_invis)); //gmon_name [mons_class [o]]);
                        strcat(info [info_lines], " appears confused.");
                        incr();
                        func_pass [1] = 1;
                }
                return info_lines;// break;



                case 5: // invisibility
                if (mons_ench [o] [2] == 6 | is_near != 1)
                {
/*                      strcpy(info [func_pass [0]], "Nothing appears to happen...");
                        func_pass [0]++;
                        return info_lines;*/
         goto nothinghap;
                }
                if (mons_ench [o] [2] != 0)
                {
//                      strcpy(info [func_pass [0]], "The ");
                        strcpy(info [info_lines], monam (mons_sec [o], mons_class [o], mons_ench [o] [2], 0)); //monam (mons_class [o], mons_ench [o] [2], 0, see_invis)); // gmon_name [mons_class [o]]);
                        strcat(info [info_lines], " flickers for a moment.");
                        incr();
                        func_pass [1] = 1;
                        return info_lines;
                }
//              strcpy(info [func_pass [0]], "The ");

                if (is_near == 1)
                {
                        strcpy(info [info_lines], monam (mons_sec [o], mons_class [o], mons_ench [o] [2], 0));//monam (mons_class [o], mons_ench [o] [2], 0, see_invis)); //gmon_name [mons_class [o]]);
                        strcat(info [info_lines], " flickers and vanishes!");

                        incr();
                        func_pass [1] = 1;
                }
                mons_ench [o] [2] = 6;
                return info_lines;

                // 6 is used by digging
  // 7            teleport
  // 8            polymorph
                case 9: // charm
                for (p = 0; p < 3; p ++)
                {
                        if (mons_ench [o] [p] == 7) return info_lines;
                } // end of for p

                for (p = 0; p < 4; p ++)
                        {
   if (p == 3) return info_lines;
                        if (mons_ench [o] [p] == 0) // replaces 3rd enchantment if all full.
                                {
                                mons_ench [o] [p] = 30;
                                mons_ench_1 [o] = 1;
    mons_beh [o] = 7;
                                break;
                                }
                        }
                // put in an exception for fungi, plants and other things you won't notice slow down.
//              strcpy(info [func_pass [0]], "The ");
                if (is_near == 1)
                {
                        strcpy(info [info_lines], monam (mons_sec [o], mons_class [o], mons_ench [o] [2], 0)); //monam (mons_class [o], mons_ench [o] [2], 0, see_invis)); //gmon_name [mons_class [o]]);
                        strcat(info [info_lines], " is charmed.");
                        incr();
                        func_pass [1] = 1;
                }
                return info_lines;// break;



                } // end of switch (beam_colour)


      goto somethinghap;


      nothinghap:
      strcpy(info [info_lines], "Nothing appears to happen.");
      incr();





      somethinghap: return info_lines;


}










void poison_monster(int mn, char source)
{

if (mons_res_poison(mons_class [mn]) != 0) return;
if (mons_inv [mn] [2] != 501 && item_dam [mons_inv [mn] [2]] % 30 == 4) return;

for (p = 0; p < 3; p ++)
                {
                        if (mons_ench [mn] [p] % 50 == 10)
                        {
                                //strcpy(info [info_lines], "Nothing appears to happen.");
                                //incr();
                                return;
                        }
                        if (mons_ench [mn] [p] % 50 >= 7 && mons_ench [mn] [p] % 50 < 10)
                        {
                                mons_ench [mn] [p] ++;
                                brek = 1;
    if (mons_near(mn) == 1 && (mons_ench [mn] [2] != 6 | see_invis != 0))
    {
        strcpy(info [info_lines], monam (mons_sec [mn], mons_class [mn], mons_ench [mn] [2], 0));
      strcat(info [info_lines], " looks rather sicker.");
      incr();
    }
    break;
                        }
                } // end of for p

                if (brek == 0)
                        for (p = 0; p < 3; p ++)
                        {
                         if (mons_ench [mn] [p] == 0) // | p == 2) // replaces 3rd enchantment if all full.
                                        {

                                                mons_ench [mn] [p] = 7;
                              mons_ench_1 [mn] = 1;
                        if (mons_near(mn) == 1 && (mons_ench [mn] [2] != 6 | see_invis != 0))
                        {
                                strcpy(info [info_lines], monam (mons_sec [mn], mons_class [mn], mons_ench [mn] [2], 0));
                         strcat(info [info_lines], " looks rather sick.");
                         incr();
                        }
                                                break;
                     }
         }

   if (source == 1 && mons_ench [mn] [p] <= 10 && mons_ench [mn] [p] >= 7)
   {
     mons_ench [mn] [p] += 50;
   }

              // put in an exception for fungi, plants and other things you won't notice slow down.
//              strcpy(info [info_lines], "The ");
                //strcpy(info [info_lines], monam (mons_sec [o], mons_class [o], mons_ench [o] [2], 0));//monam (mons_class [o], mons_ench [o] [2], 0, see_invis)); //gmon_name [mons_class [o]]);
                //strcat(info [info_lines], " seems to slow down.");
                //func_pass [0]++;
      //incr();
                //func_pass [1] = 1;



}


char create_monster(int cls, int dur, int beha, int cr_x, int cr_y, int hitting)
{
//x_plos = x_pos;
//y_plos = y_pos;
int pets = 0;
int summd = 0;
/*
char minx = -1;
char maxx = 3;
char miny = -1;
char maxy = 2;
char xinc = 1;
char yinc = 1;

if (random2(2) == 0)
{
        minx = 1; maxx = -3; xinc = -1;
}

if (random2(2) == 0)
{
        miny = 1; maxy = -2; yinc = -1;
}


for (count_x = minx; count_x != maxx; count_x += xinc)
{

   if (count_x == 2 | count_x == -2)
   {
 */

if (empty_surrounds(cr_x, cr_y) == 0)
{
 puffy :
 if (see_grid(cr_x, cr_y) == 1)
 {
  strcpy(info [info_lines], "You see a puff of smoke.");
  incr();
 }
 return 0;
} else
       {
/*
   for (count_y = miny; count_y != maxy; count_y += yinc)
   {

      if (count_x == 0 && count_y == 0) continue;

      if (grid [x_plos + count_x] [y_plos + count_y] > 10 && grid [x_plos + count_x] [y_plos + count_y] < 50)
      {*/
         summd = mons_place(cls, 1, empty_x, empty_y, beha, hitting);
         if (summd == -1) goto puffy;
         goto bkout;
       } // end else

//   } // end of count_y
//} // end of count_x

bkout : pets = 0;

mons_ench [summd] [1] = dur; // some monsters, eg butterflies, use [0] for confusion
mons_ench_1 [summd] = 1;

return 1;

}



char empty_surrounds(int emx, int emy)
{

char minx = -1;
char maxx = 3;
char miny = -1;
char maxy = 2;
char xinc = 1;
char yinc = 1;

if (random2(2) == 0)
{
        minx = 1; maxx = -3; xinc = -1;
}

if (random2(2) == 0)
{
        miny = 1; maxy = -2; yinc = -1;
}


for (count_x = minx; count_x != maxx; count_x += xinc)
{

   if (count_x == 2 | count_x == -2)
   {
     return 0;
   }

   for (count_y = miny; count_y != maxy; count_y += yinc)
   {

      if (count_x == 0 && count_y == 0) continue;

                                   // Should this % 100 be here?
      if (grid [emx + count_x] [emy + count_y] % 100 >= 17 && grid [emx + count_x] [emy + count_y] % 100 < 50)
      {
         goto bkout;
      }

   } // end of count_y
} // end of count_x

bkout:
empty_x = emx + count_x;
empty_y = emy + count_y;
return 1;

} // end empty surrounds




char curse_an_item(char which, char power)
{
// use which later, if I want to curse weapon/gloves whatever
// which, for now: 0 = non-mummy, 1 = mummy (potions as well)
// don't change item_dam of !odecay

// I think this is wrong

char possib [52];
char cu = 0;
char cu1 = 0;
char cu2 = 0;
char cu3 = 0;


for (cu = 0; cu < 52; cu++)
{
  possib [cu] = 100;
}

for (cu = 0; cu < 52; cu++)
{
  if (inv_quant [cu] == 0) continue;

  cu1 = inv_class [cu];

//  if (cu1 == 1) continue;

  if (cu1 == 0 | cu1 == 2 | cu1 == 7)
  {
   if (inv_plus [cu] >= 130) continue;
   possib [cu2] = cu;
   cu2++;
  }

  if (cu1 == 8 && which == 1)
  {
    if (inv_type [cu] == 15) continue;
    possib [cu2] = cu;
    cu2++;
  }

}

if (cu2 == 0) return 0;

//char counting = 0;

choose : do
           {
              cu3 = random2(cu2);
           } while (possib [cu3] == 100);

//counting ++;
//if (counting > 10) return 0;

if (inv_class [possib [cu3]] == 8)
{
  //if (inv_type [cu3] == 15) goto choose; //return 0; // can't do goto choose because of infinite loop if only have !odecay

  inv_type [possib [cu3]] = 15; // don't change inv_dam (just for fun)
  return 1;
  // problem: changes large piles of potions
}

//if (inv_plus [cu3] > 130) goto choose;

inv_plus [possib [cu3]] += 100;

//decay == 15

return 1;

}

//void monster_beckon(int beck

void monster_blink(int mn)
{

// I can't be bothered writing an intelligent function, so I'll make it ugly:

if (mons_class [mn] == 19 | mons_class [mn] == 56) return; // Do not let this happen!

int passed [2];

if (random_near_space(passed) == 0) return;

grid [mons_x [mn]] [mons_y [mn]] -= 50;

mons_x [mn] = passed [0];
mons_y [mn] = passed [1];

grid [mons_x [mn]] [mons_y [mn]] += 50;

}

char random_near_space(int passed [2])
{

passed [0] = 0;
passed [1] = 0;

int blx = passed [0];
int bly = passed [1];
int tries = 0;

do
{

blx = x_pos - 6 + random2(14);
bly = y_pos - 6 + random2(14);

tries ++;
if (tries >= 150) return 0;
} while (see_grid(blx, bly) == 0 | grid [blx] [bly] < 17 | grid [blx] [bly] >= 50 | (x_pos == blx && y_pos == bly));
                                                        // 17???
passed [0] = blx;
passed [1] = bly;

return 1;
}


void unwield_item(char unw)
{

if (inv_class [unw] == 0 && inv_dam [unw] > 180)
{
 switch(inv_dam [unw] - 180)
 {

  case 1:
  strcpy(info [info_lines], "The Singing Sword sighs.");
  incr();
  break;

  case 2:
  strcpy(info [info_lines], "You feel less violent.");
  incr();
  break;

 }
 special_wield = 0;
 return;
}

if (inv_class [unw] == 0 && inv_dam [unw] != 0)
{

if (inv_ident [unw] == 1) inv_ident [unw] = 2;

item_name(inv_class [unw], inv_type [unw], inv_dam [unw], inv_plus [unw], inv_quant [unw], inv_ident [unw], 4, str_pass);
strcpy(info [info_lines], str_pass);

switch(inv_dam [unw] % 30)
{

  case 1:
  strcat(info [info_lines], " stops flaming.");
  incr();
  break;

  case 2:
  strcat(info [info_lines], " stops glowing.");
  incr();
  break;

  case 3:
  strcat(info [info_lines], " stops glowing.");
  incr();
  break;

  case 4:
  strcat(info [info_lines], " stops crackling.");
  incr();
  break;

/*  case 5:
  strcpy(info [info_lines], "You feel a sudden desire to kill orcs!");
  incrl();
  break;
*/
  case 6:
  strcat(info [info_lines], " stops dripping with poison.");
  incr();
  break;

  case 7:
  strcpy(info [info_lines], "You feel less protected.");
  incr();
                AC -= 3;
         AC_ch = 1;
                break;

  // case 8: draining

} // end of switch

}


if (inv_class [item_wielded] == 11)
{
 switch(inv_type [item_wielded])
        {
 case 0: // wizardry
 mag_abil -= 30;
        break;

 case 1: // magic
 ep_max -= 13;
 if (ep > ep_max) ep = ep_max;
 ep_ch = 1;
 magic_battery --;
 break;

 case 2: // fire
        spec_fire --;
        break;

        case 3: // cold
 spec_cold --;
        break;

 case 4:
 spec_poison --;
        break;

 case 5:
 energy --;
        break;
        }
} // end if inv_cla == 11


} // end of unwield_item



void unwear_armour(char unw)
{

if (inv_type [unw] == 8 | inv_type [unw] == 13 | inv_type [unw] == 14)//shield
{
  shield_class = 0;
} else
{
AC -= property [inv_class [unw]] [inv_type [unw]] [0] + inv_plus [unw] - 50;
if (clas == 0 | clas == 2) // fighter or priest
{
 evasion -= property [inv_class [unw]] [inv_type [unw]] [1] / 2;
// if (inv_type [armour_wear_2] == 8) evasion -= 1;
} else
      {
                                                        evasion -= property [inv_class [unw]] [inv_type [unw]] [1];
//                   if (inv_type [armour_wear_2] == 8) evasion += 1;
      }
} // end of if !shield
AC_ch = 1;
evasion_ch = 1;

switch(inv_dam [unw] % 30)
{

case 1:
  strcpy(info [info_lines], "You feel rather sluggish.");
  incr();
  fast_run --;
break;

  case 2:
  res_fire ++;
  if (res_fire == 0)
                {
          strcpy(info [info_lines], "You feel less resistant to fire.");
          incr();
  }
  break;

  case 3:
  res_cold ++;
                if (res_cold == 0)
                {
          strcpy(info [info_lines], "You feel less resistant to cold.");
          incr();
  }
  break;

  case 4:
  res_poison ++;
                if (res_poison == 0)
                {
          strcpy(info [info_lines], "You feel less healthy.");
          incr();
                }
  break;

  case 5:
  see_invis ++;
                if (see_invis == 0)
                {
          strcpy(info [info_lines], "You feel less perceptive.");
          incr();
                }
  break;

 case 6: // invisibility
 invis ++;
        if (invis == 0) invis = 1;
        hunger_inc -= 7;
        break;

 case 7:
 strcpy(info [info_lines], "You feel weak.");
 incr();
 strength -= 3;
 max_strength -= 3;
 strength_ch = 1;
 break;

 case 8:
 strcpy(info [info_lines], "You feel clumsy.");
 incr();
 dex -= 3;
 max_dex -= 3;
 dex_ch = 1;
 break;

 case 9:
 strcpy(info [info_lines], "You feel dopey."); // inspired messages, aren't they?
 incr();
 intel -= 3;
 max_intel -= 3;
 intel_ch = 1;
 break;

 case 10:
 strcpy(info [info_lines], "You feel rather less ponderous.");
 speed -= 2;
 evasion += 1;
 evasion_ch = 1;
 break;

        case 11: // levitation
        lev ++;
        if (lev == 0) lev = 1;
        break;

 case 12:
 strcpy(info [info_lines], "You feel less resistant to magic.");
 incr();
 res_magic -= 20;
 break;


} // end of switch

} // end of unwear_armour



int distance(int x1, int x2, int y1, int y2)
{
// not accurate (at all), but its main purpose is to see whether you're next to something or not.
int xdisp = x1 - x2;
if (xdisp < 0) xdisp *= -1;

int ydisp = y1 - y2;
if (ydisp < 0) ydisp *= -1;

if (xdisp == 1 && ydisp == 1) ydisp = 0;
//itoa(xdisp + ydisp, st_prn, 10);
//strcpy(info [info_lines], st_prn);
//incr();

return xdisp + ydisp;

}



void swap_places(int swap)
{

strcpy(info [info_lines], "You swap places.");
incr();

grid [mons_x [swap]] [mons_y [swap]] -= 50;

mons_x [swap] = x_pos;
mons_y [swap] = y_pos;

grid [mons_x [swap]] [mons_y [swap]] += 50;

} // end of swap_places






void place_cloud(unsigned char cl_type, unsigned char ctarget_x, unsigned char ctarget_y, unsigned char cl_range) // if cloud_type > 100, it is a monster's cloud
{
 int ci = 0;

 char c_deleted = 100;

 if (cloud_no == 30) c_deleted = random2(30);

 if (grid [ctarget_x] [ctarget_y] >= 100) return; // hmmm...

        for (ci = 0; ci < CLOUDS; ci++)
        {
  if (ci == c_deleted)
  {
   cloud_type [ci] = 0;
   grid [cloud_x [ci]] [cloud_y [ci]] -= 100;
   cloud_decay [ci] = 0;
   cloud_no--;
  }

                if (cloud_type [ci] == 0) // ie is empty
                {
                        cloud_type [ci] = cl_type;
                        cloud_x [ci] = ctarget_x;
                        cloud_y [ci] = ctarget_y;
                        cloud_decay [ci] = cl_range * 10;
                        cloud_no++;
                        grid [cloud_x [ci]] [cloud_y [ci]] += 100;
                        break;
                }

 }

//      move_x = 0; move_y = 0;

} // end of void place_cloud();

void alert2(void) // different - takes account of invis
{
int alrt = 0;

for (alrt = 0; alrt < MNST; alrt ++)
{
 if (mons_class [alrt] != -1 && mons_near(alrt) == 1)
 {
        if (mons_beh [alrt] == 0)
                mons_beh [alrt] = 1;
  if (invis == 0 | mons_see_invis(mons_class [alrt]) > 0)
  {
      mons_targ_1_x [alrt] = x_pos;
      mons_targ_1_y [alrt] = y_pos;
  }
 }
} // end for

} // end void alert()


char detect_traps(void)
{
char traps_found = 0;

  strcpy(info [info_lines], "You detect traps!");
                incr();

  for (count_x = 0; count_x < NTRAPS; count_x ++)
                {
//    if (see_grid(trap_x [count_x], trap_y [count_x]) == 1)
//                              {
    if (trap_x [count_x] > x_pos - 15 && trap_x [count_x] < x_pos + 15 && trap_y [count_x] > y_pos - 8 && trap_y [count_x] < y_pos + 8)
    {
        if (grid [trap_x [count_x]] [trap_y [count_x]] % 50 == 28)
                                        {
       grid [trap_x [count_x]] [trap_y [count_x]] -= 3;
       map [trap_x [count_x] - 1] [trap_y [count_x] - 1] = '^';
       traps_found++;
     }
                                }
  }

return traps_found;

/*              case 25: buffy [bufcount + 1] = 11;
                showed = 94; break; // ^ dart trap
                case 26: buffy [bufcount + 1] = LIGHTGRAY;
                showed = 94; break;
                case 27: buffy [bufcount + 1] = MAGENTA;
                showed = 94; break;
                case 28: showed = 249; break; // undiscovered trap
*/

}


char you_resist_magic(int power)
{
/*  itoa(power, st_prn, 10);
                strcpy(info [info_lines], st_prn);
  incr();
  itoa(res_magic, st_prn, 10);
                strcpy(info [info_lines], st_prn);
  incr();*/
 if (random2(power) / 3 + random2(power) / 3 + random2(power) / 3 >= res_magic) return 0;
 return 1;
}


void forget_map(void)
{
char xcount = 0;
char ycount = 0;

for (xcount = 0; xcount < 80; xcount ++)
{
 for (ycount = 0; ycount < 70; ycount ++)
 {
  map [xcount] [ycount] = 0;
 }
}


}

void how_hungered(int hunge)
{
if (hunge <= 100)
{
 strcpy(info [info_lines], "You feel slightly");
 goto full_or_hungry;
}
if (hunge <= 350)
{
 strcpy(info [info_lines], "You feel somewhat");
 goto full_or_hungry;
}
if (hunge <= 800)
{
 strcpy(info [info_lines], "You feel a quite a bit");
 goto full_or_hungry;
}
 strcpy(info [info_lines], "You feel a lot");

full_or_hungry:
if (hung_state >= 4)
{
 strcat(info [info_lines], " less full.");
 incr();
 return;
}
strcat(info [info_lines], " more hungry.");
incr();
return;

}

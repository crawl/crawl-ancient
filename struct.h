
struct player
{
char your_name [30];

unsigned char species;

char teleport;
char see_invis;
char res_fire;
char res_poison;
char res_cold;
char fast_run;
char sust_abil;
char deaths_door;
//char hiding = 1;
char magic_battery;
char spec_fire;
char spec_cold;
char spec_poison;
char prot_life;
char special_wield;
char berserker;
char energy;
unsigned char corpse_count;
unsigned char disease;
char spec_death;
char spec_holy;
char spec_conj;
char spec_ench;
char spec_summ;

//char searching;

char level_type ;
// 0 = normal dungeon level
// 1 = labyrinth
// 2 = abyss
// 3 = pandemonium

int res_magic ;
char max_level;
int x_pos;
int y_pos;
  char unique_items [50];
int invis ;
int conf;
int paralysis;
int slow ;
int haste ; // haste
int might ; // might
int lev ; // levitation
int poison ; // poison!!
int rotting ;
unsigned char shock_shield ;
int hunger;
char hunger_inc;
char armour [6];
char ring [3];
int item_wielded;
int hp;
int hp_max;
int base_hp; // this is what is affected by necrophages etc
int base_hp2;
int ep;
int ep_max;
int base_ep;
int base_ep2;
char strength;
char intel;
char dex;
char max_strength;
char max_intel;
char max_dex;
int AC ;
int evasion;
char shield_class ;
int damage;
char rate_regen;
unsigned char incr_regen ;
char ep_rate_regen;
unsigned char ep_incr_regen ;
unsigned long xp ;
int xl;
unsigned int gp ;
int clas;
char clasnam [30];
int f_abil; // fighting ability, out of whatever
int mag_abil; // magic ability, out of whatever
int thr_abil; // throw_abil
char speed; // 0.75;
int speed_inc ; // This variable isn't used - must remove it sometime.
        unsigned char inv_class [52];
        unsigned char inv_type [52];
        unsigned char inv_plus [52];
        unsigned char inv_plus2 [52];
        unsigned char inv_dam [52];
        char inv_col [52];
        int inv_quant [52]; // multiple items, eg ammo or potions
        int burden; // total weight of items carried.
 char burden_state ;
        char inv_no; // number of items carried.
        unsigned char inv_ident [52];
unsigned char spells [25];
char spell_no;
char spell_levels;
unsigned char char_direction ;
// 0 = going down
// 1 = going up!
unsigned char where_are_you ;
// 0 = normal dungeon level
// 1 = dis
// 2 = hell
unsigned char item_description [5] [50];
unsigned char pet_target;
int your_level;
char levels_exist [30];
unsigned char duration [30]; // lots of durational things. Why didn't I do this for haste etc right from the start? Oh well.
/*
duration:
0 - liquid flames
1 - icy armour
2 - deflect missiles
3 - prayer
4 - regeneration
5 - vorpal blade
6 - fire brand
7 - ice brand
8 - lethal infusion
9 - swiftness
10 - insulation
11 - stonemail
12 - controlled flight
13 - teleport
14 - teleport control
15 - species ability
*/
unsigned char attribute [10]; // various attributes, eg resist lightning
/*
0 - resist lightning
1 - spec_air
2 - spec_earth
3 - teleport control
4 - slow (move only)
*/


unsigned char piety;
unsigned char religion;

unsigned char skills [50];
unsigned char practise_skill [50];
unsigned int skill_points [50];
int exp_available;

char is_undead;
// 0 - is alive
// 1 - is undead, but can still eat/drink etc (eg vampire)
// 2 - is undead (mummy etc)


};

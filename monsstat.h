//#define MLAVA0 220
//#define MLAVA1 221
#define MNST 200
//void moname(char mcl, char mench, char see_inv, char descrip, char glog [40]);

int mondamage(int mc, int rt);

void mon_init(char gmon_use [1000], char mcolour [1000]);

void def_letters(char letters [52] [1]);

void def_properties(int property [4] [50] [3], int mass [20] [50]);

int mon_resist_mag(int mc, char mhd);

//void m_all(int mons_alloc [10], int curr_level);

int mons_res_fire(int mclass);

int mons_intel(int mclass);

int mons_res_poison(int mclass);

int mons_zombie_size(int mclass);

int mons_weight(int mclass);
int mons_corpse_thingy(int mclass);
int mons_charclass(int mcls);

int mons_res_cold(int mclass);

int mons_holiness(int mclass);

char mons_shouts(int mclass);

char mons_flies(int mclass);

void mons_spell_list(unsigned char sec, int splist [6]);

void replay_messages(char info [78] [78]);

char mons_see_invis(int mclass);

char damage_type(char wclass, char wtype);

void define_monster(int k, struct monsters mns [MNST]);

int mons_exp_mod(int mclass);

char mons_organs(int mclass);

int mons_skeleton(int mcls);

int mons_res_elec(int mclass);

int hit_points(int hit_dice, int min_hp, int rand_hp);

char weapon_skill(char wclass, char wtype);

char mons_pan(int mcls);

char weapon_speed(char wclass, char wtype);


unsigned char mons_char(int mc);
int mons_flag(int mc,int bf);

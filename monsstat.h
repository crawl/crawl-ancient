#define MLAVA0 220
#define MLAVA1 221

int mondamage(int mc, int rt);

void mon_init(char gmon_use [400], char mcolour [400]);

void def_letters(char letters [52] [1]);

void def_properties(int property [4] [30] [3], int mass [15] [30]);

int mon_resist_mag(int mc, char mhd);

//void m_all(int mons_alloc [10], int curr_level);

int mons_res_fire(int mclass);

int mons_intel(int mclass);

int mons_res_poison(int mclass);

int mons_zombie_size(int mclass);

int mons_res_cold(int mclass);

int mons_holiness(int mclass);

char mons_shouts(int mclass);

char mons_flies(int mclass);

char mons_spell_list(int sec, int whichs);

void replay_messages(char info [78] [78]);

char mons_see_invis(int mclass);

char damage_type(char wclass, char wtype);

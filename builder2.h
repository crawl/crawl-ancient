#define ITEMS 500
#define MNST 200
#define NTRAPS 30
#define CLOUDS 30
//#include <dos.h>

#define MLAVA0 220
#define MLAVA1 221
#define MLAVA2 222
#define MLAVA3 223
#define MLAVA4 224
#define MLAVA5 225

#define MWATER0 230
#define MWATER1 231
#define MWATER2 232
#define MWATER3 233
#define MWATER4 234
#define MWATER5 235






int builder(
int igrid [80] [70],
unsigned char grid [80] [70],
int mons_class [MNST],
int mons_hp [MNST],
int mons_hp_max [MNST],
unsigned char mons_HD [MNST],
int mons_AC [MNST],
char mons_ev [MNST],
unsigned char mons_speed [MNST],
unsigned char mons_speed_inc [MNST],
unsigned char mons_x [MNST],
unsigned char mons_y [MNST],
        unsigned char mons_beh [MNST],
        int mons_inv [MNST] [8],
        unsigned char mons_targ_1_x [MNST],
        unsigned char mons_targ_1_y [MNST],
        unsigned char mons_sec [MNST],
 unsigned char mons_ench_1 [MNST],
 unsigned char mons_ench [MNST] [3],
 unsigned char mons_hit [MNST],
int no_it,
unsigned char item_ident [ITEMS],
unsigned char item_class [ITEMS],
char unique_items [50],
unsigned char item_type [ITEMS],
unsigned char item_plus [ITEMS],
unsigned char item_dam [ITEMS],
int item_quant [ITEMS],
unsigned char item_x [ITEMS],
unsigned char item_y [ITEMS],
unsigned char icolour [ITEMS],
int item_link [ITEMS],
unsigned char item_description [5] [30],
int property [4] [30] [3],
unsigned char trap_type [NTRAPS],
unsigned char trap_x [NTRAPS],
unsigned char trap_y [NTRAPS],
unsigned char mons_alloc [20],
char gmon_use [200],
unsigned char stair_x [10],
unsigned char stair_y [10],
unsigned int lev_numb,
int x_pos,
int y_pos,
char level_type);


void define_monster(int mons_class [MNST],
int mons_hp [MNST],
int mons_hp_max [MNST],
unsigned char mons_HD [MNST],
int mons_AC [MNST],
char mons_ev [MNST],
unsigned char mons_speed [MNST],
unsigned char mons_speed_inc [MNST],
unsigned char mons_sec [MNST], int k);



int place_monster(int mons_class [MNST],
int mons_hp [MNST],
int mons_hp_max [MNST],
unsigned char mons_HD [MNST],
int mons_AC [MNST],
char mons_ev [MNST],
unsigned char mons_speed [MNST],
unsigned char mons_speed_inc [MNST],
int mons_inv [MNST] [8],
unsigned char mons_sec [MNST],
unsigned char mons_ench_1 [MNST],
unsigned char mons_ench [MNST] [3],
unsigned char mons_hit [MNST],
unsigned char mons_beh [MNST],
int no_it,
unsigned char item_ident [ITEMS],
unsigned char item_class [ITEMS],
char unique_items [50],
unsigned char item_type [ITEMS],
unsigned char item_plus [ITEMS],
unsigned char item_dam [ITEMS],
int item_quant [ITEMS],
unsigned char item_x [ITEMS],
unsigned char item_y [ITEMS],
unsigned char icolour [ITEMS],
unsigned char xtra_quant,
int item_link [ITEMS],
unsigned char item_description [5] [30],
int property [4] [30] [3],
unsigned char mons_x [MNST],
unsigned char mons_y [MNST],
char plus_seventy,
int typed, int type_place, int px, int py, char behaviour, int hitting,
char allow_bands,
char gmon_use [200],
unsigned char grid [80] [70],
unsigned char mons_alloc [20],
int passed [2],
int igrid [80] [70],
int x_pos,
int y_pos);


void spellbook_template(int sbook_type, int func_pass [10]);


/*

int place_monster(int mons_class [MNST],
int mons_hp [MNST],
int mons_hp_max [MNST],
unsigned char mons_HD [MNST],
int mons_AC [MNST],
char mons_ev [MNST],
unsigned char mons_speed [MNST],
unsigned char mons_speed_inc [MNST],
int mons_inv [MNST] [8],
unsigned char mons_sec [MNST],
unsigned char mons_ench_1 [MNST],
unsigned char mons_ench [MNST] [3],
unsigned char mons_hit [MNST],
unsigned char mons_beh [MNST],
int no_it,
unsigned char item_ident [ITEMS],
unsigned char item_class [ITEMS],
unsigned char item_type [ITEMS],
unsigned char item_plus [ITEMS],
unsigned char item_dam [ITEMS],
int item_quant [ITEMS],
unsigned char item_x [ITEMS],
unsigned char item_y [ITEMS],
unsigned char icolour [ITEMS],
unsigned char xtra_quant,
int item_link [ITEMS],
unsigned char item_description [5] [30],
int property [4] [30] [3],
unsigned char mons_x [MNST],
unsigned char mons_y [MNST],
char plus_seventy,
int typed, int type_place, int px, int py, char behaviour, int hitting,
char allow_bands
);
  */

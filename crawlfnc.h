#define ITEMS 500
#define MNST 200
#define CLOUDS 30
#define NTRAPS 30

extern int dir_cursor(char rng);
extern void draw_border(int bord_col, char your_name [9], char clasnam [13]);
extern char *ident(char clss, char typ, unsigned char plu); // char dam) - not yet


//char mons_define(char mclss, char chrct);
char mons_define(int mclss, unsigned char chrct, unsigned char mons_sec);

//void losight(unsigned char sh [15] [15], unsigned char gr [80] [70], int x_pos, int y_pos);


void messsage(char inf [60] [78], char inf_lines);

void mmore(void);

char new_game(char char_name [8]);

//extern void unidentify(char fak_name [ITEMS] [30], char item_clas [ITEMS], char item_typ [ITEMS], unsigned char item_da [ITEMS]);

int exper_value(int mclass, char mHD, char maxhp);

void replace_items(char it_class [ITEMS], char it_type [ITEMS], unsigned char it_dam [ITEMS], char it_col [ITEMS], unsigned char it_plus [ITEMS], unsigned int it_quant [ITEMS], int levl, unsigned char gr [80] [70], unsigned char it_x [ITEMS], unsigned char it_y [ITEMS]);

//char *moname(char mcl, char mench, char see_inv, char descrip);

void moname(int mcl, char mench, char see_inv, char descrip, char glog [40]);

char restock_monsters(int mo_class [MNST], int mo_inv [MNST] [8], char it_class [ITEMS], char it_type [ITEMS], unsigned char it_dam [ITEMS], char it_col [ITEMS], unsigned char it_plus [ITEMS], unsigned int it_quant [ITEMS], int levl, unsigned char gr [80] [70], unsigned char it_x [ITEMS], unsigned char it_y [ITEMS], int n_it, char k, char mo_x [MNST], char mo_y [MNST], int ilink [ITEMS], char it_id [MNST]);

void monsters_find(int mo_class [MNST], int mo_inv [MNST] [8], char it_class [ITEMS], char it_type [ITEMS], unsigned char it_dam [ITEMS], char it_col [ITEMS], unsigned char it_plus [ITEMS], unsigned int it_quant [ITEMS], int levl, unsigned char gr [80] [70], unsigned char it_x [ITEMS], unsigned char it_y [ITEMS], int n_it, char k, char mo_x [MNST], char mo_y [MNST], int proper [4] [30] [3], int mo_AC [MNST], char mo_ev [MNST]);

void item_cull(int mo_class [MNST], int mo_inv [MNST] [8], char it_class [ITEMS], char it_type [ITEMS], unsigned char it_dam [ITEMS], char it_col [ITEMS], unsigned char it_plus [ITEMS], unsigned int it_quant [ITEMS], int levl, unsigned char gr [80] [70], unsigned char it_x [ITEMS], unsigned char it_y [ITEMS], int n_it, char mo_x [MNST], char mo_y [MNST]);

char item_name(char item_clas, char item_typ, unsigned char item_da, unsigned char it_plus, unsigned int it_quant, char ident_lev, char descrip, char glag [60]);

//void monsters_get(int mo_class [MNST], int mo_inv [MNST] [8], char it_class [ITEMS], char it_type [ITEMS], unsigned char it_dam [ITEMS], char it_col [ITEMS], char it_plus [ITEMS], unsigned int it_quant [ITEMS], int levl, unsigned char gr [80] [70], unsigned char it_x [ITEMS], unsigned char it_y [ITEMS], int n_it, char k, char mo_x [MNST], char mo_y [MNST]);

void monsters_get(int mo_class [MNST], int mo_inv [MNST] [8], char it_class [ITEMS], char it_type [ITEMS], unsigned char it_dam [ITEMS], char it_col [ITEMS], unsigned char it_plus [ITEMS], unsigned int it_quant [ITEMS], int levl, unsigned char gr [80] [70], unsigned char it_x [ITEMS], unsigned char it_y [ITEMS], int n_it, char k, char mo_x [MNST], char mo_y [MNST], int mo_AC [MNST], char mo_ev [MNST], int proper [4] [30] [3]);

char zappy(int func_pass [10], char str_pass [50], char z_type);

char manage_clouds(char cloud_x [CLOUDS], char cloud_y [CLOUDS], char cloud_type [CLOUDS], int cloud_decay [CLOUDS], int cloud_no, unsigned char grid [80] [70], char time_taken, int haste, int slow);

int mons_ench_f2(char info [78] [78], char info_lines, unsigned char mons_speed [MNST], char mons_ench [MNST] [3], int o, char mons_ench_1 [MNST], int mons_hp [MNST], int mons_hp_max [MNST], int mons_class [MNST], char is_near, char beam_colour, char see_invis, int func_pass [10]);

char *monam(int mons_cla, char mons_e, char desc, char see_invis);

//void spellbook_template(int sbook_type, int func_pass [10]);

/*
int builder(
int igrid [80] [70],
unsigned char grid [80] [70],
int mons_class [MNST],
int mons_hp [MNST],
int mons_hp_max [MNST],
unsigned char mons_HD [MNST],
int mons_AC [MNST],
unsigned char mons_ev [MNST],
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
unsigned int lev_numb);

  */

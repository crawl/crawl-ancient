#define ITEMS 500
#define MNST 200

char message(char inf [60] [78], int inf_lines, char scrloc);

char item_name_2(char item_clas, char item_typ, unsigned char item_da, char it_plus, unsigned int it_quant, char ident_lev, char glog [60]);

void initial(void);

void set_id(char cla, char ty, char setting);

char get_id(char cla, char ty);

void housekeep(int mons_class [MNST], int mons_hp [MNST], unsigned char mons_hp_max [MNST], unsigned char mons_HD [MNST], int mons_AC [MNST], char mons_ev [MNST], float mons_speed [MNST], float mons_speed_inc [MNST], unsigned char mons_x [MNST], unsigned char mons_y [MNST], unsigned char mons_targ_1_x [MNST], unsigned char mons_targ_1_y [MNST], int mons_inv [MNST] [8], unsigned char mons_beh [MNST]);

//void trap_item(unsigned char item_dam [ITEMS], unsigned char item_plus [ITEMS], char item_type [ITEMS], char item_class [ITEMS], int item_link [ITEMS], int item_quant [ITEMS], int igrid [80] [70], char iclass, char itype, char beam_x, char beam_y);
int trap_item(int it_no, char icolour [ITEMS], char item_ident [ITEMS], unsigned char item_dam [ITEMS], unsigned char item_plus [ITEMS], char item_type [ITEMS], char item_class [ITEMS], int item_link [ITEMS], unsigned int item_quant [ITEMS], int igrid [80] [70], char iclass, char itype, char beam_x, char beam_y);

void put_show(int count_x, int count_y, int x_pos, int y_pos, unsigned char show_col [15] [15], unsigned char show [15] [15], int your_colour, int your_sign);

int eating(char info [78] [78], int infls, int func_pass [10], int food_eat_3); //int hung, int delay_t, int delay_doing, int food_eat_3);

char *weird_colours(unsigned char coll);

int mons_spells(char spell_cast, int func_pass [10], char beam_name [30]);

void monster_grid(unsigned char no_mons, unsigned char mons_x [MNST], unsigned char mons_y [MNST], unsigned char mons_beh [MNST], int invis, char see_invis, int mons_class [MNST], char mcolour [MNST], int x_pos, int y_pos, unsigned char show [15] [15], unsigned char show_col [15] [15], char mons_ench [MNST] [3], int func_pass [10], unsigned char mons_sec [MNST]);

char get_ch(void);

//void mons_ench_f(char mons_ench [MNST] [3], char mons_speed [MNST], char mons_ench_1 [MNST], char mons_beh [MNST], int i);

int exp_needed(int lev);

int random2(unsigned int rmax);

int magic_ability(int mag_abil, int intel);

//void outp(int retv);

unsigned char check_item_knowledge(void);


//void zapping(char ztype, char beam_colour, char beam_name [30], char beam_range, unsigned char beam_damage, char beam_flavour, char beam_type, char thing_thrown, char beam_hit);

//void zapping(char ztype, char beam_colour, char beam_name [30], int beam_range, int beam_damage, char beam_flavour, int beam_type, int thing_thrown, int beam_hit);



//char incrline(void);

//void zeroinf(void);


char spell_list(char spells [25]);

char spell_value(char spell);

void spell_name(char spell, char spln [60]);

char spellbook_contents(unsigned char plus, unsigned char type);

int which_spell_in_book(int sbook_type, int spl);

int spell_hunger(int spell_value, char spell);

void replay_messages(void);

void save_id(char identy [4] [30]);

/*
This is a list of the various .h files. Functions which are not in .h files
are not listed, and this file may well be out of date.
*/


/* bang.cc */
void explosion1(struct bolt beam [1]);
void explosion(char ex_size, struct bolt beam [1]);

/* beam.cc */
void missile(struct bolt beam [1], int throw_2);
int check_mons_resists(struct bolt beam [1], int o, int hurted);
void beam(struct bolt beam [1]);
int check_mons_magres(int mn, int pow);
void mass_enchantment(int wh_enchant, int pow);
int mons_ench_f2(int o, char is_near, int func_pass [10], struct bolt beam [1]);
void poison_monster(int mn, char source);
void sticky_flame_monster(int mn, char source, int power);
void place_cloud(unsigned char cl_type, unsigned char ctarget_x, unsigned char ctarget_y, unsigned char cl_range);
void tracer_f(int i, struct bolt beem [1]);

/* chardump.cc */
char dump_char(char show_prices, char fname [30]);

/* command.cc */
void stethoscope(int mwh);
void quit_game(void);
void version(void);
void adjust(void);

/* debug.cc */
void cast_spec_spell(void);
void create_spec_monster(void);
void level_travel(void);
void create_spec_object(void);
void create_spec_object2(void);
void stethoscope(int mwh);

/* decks.cc */
void deck_of_wonders(void);
void deck_of_summonings(void);

/* describe.cc */
void describe_monsters(int class_described);
void describe_item(int item_class, int item_type, int item_plus, int item_plus2, int item_dam, unsigned char item_id); //, int property [4] [50] [3], int mass [20] [50]);
void describe_spell(int spelled);

/* direct.cc */
void direction(char rnge, struct dist moves [1]);
int dir_cursor(char rng);
int look_around(struct dist moves [1]);

/* dungeon.cc */

int builder(unsigned int lev_numb, char level_type);


int place_monster(
unsigned char plus_seventy,
int typed, int type_place, int px, int py, char behaviour, int hitting,
char allow_bands,
int many_many
);


void spellbook_template(int sbook_type, int func_pass [10]);


int items(
unsigned char allow_uniques,
int force_class,
int force_type,
int force_place,
int many_many,
int force_spec
);

/* effects.cc */
void torment(void);
void banished(void);
char forget_spell(void);
char lose_stat(char force_stat, char stat_loss);
void random_uselessness(unsigned char ru, unsigned char sc_read_2);
void acquirement(void);
char recharge_wand(void);
void direct_effect(struct bolt beam [1]);
void mons_direct_effect(struct bolt beam [1], int i);

/* fight.cc */
void monster_die(int monster_killed, char killer, int i);
void you_attack(int monster_attacked);
void monster_attack(int monster_attacking);
char monsters_fight(int monster_attacking, int monster_attacked);
int jelly_divide(int jel);
void alert(void);
void monster_polymorph(unsigned char monsc, unsigned char targetc, int power);
void monster_drop_ething(int monster_killed);

/* files.cc */
void load(unsigned char stair_taken, char moving_level, char level_saved, char was_a_labyrinth, char old_level, char want_followers, char just_made_new_lev);
void save_level(int level_saved, char was_a_labyrinth);

void save_game(void);
void restore_game(void);

void reset_ch(void);

/* food.cc */
void food_change(void);
void eaten(int food_eat_2, int food_eat_3);
void eat_meat(int fod_eat_2);
char eat_from_floor(void);
char butchery(void);

/* invent.cc */
char invent(int item_class_inv, char show_price);

unsigned char get_invent(int invent_type);

void list_commands(void);

/* it_use2.cc */
void zapping(char ztype, int power, struct bolt beams [1]);
void potion_effect(char pot_eff, int pow);
void unwield_item(char unw);
void unwear_armour(char unw);
void skill_manual(char sc_read_2);
void tome_of_power(char sc_read_2);

/* it_use3.cc */
void ball_of_energy(void);
void box_of_beasts(void);
void skill_manual(char sc_read_2);
void tome_of_power(char sc_read_2);
void staff_spell(char zap_device_2);
void ball_of_seeing(void);
void efreet_flask(void);
void invoke_wielded(void);
void special_wielded(void);

/* item_use.cc */
void wield_weapon(void);
void wear_armour(void);
void takeoff_armour(void);

void throw_anything(void);
void shoot_thing(void);
void puton_ring(void);
void remove_ring(void);

void zap_wand(void);
void eat_food(void);
void drink(void);

void read_scroll(void);
void original_name(void);

/* itemname.cc */
char item_name(unsigned char item_plus2, char item_clas, char item_typ, unsigned char item_da, unsigned char it_plus, unsigned int it_quant, char ident_lev, char descrip, char glag [60]);
char item_name_2(unsigned char item_plus2, char item_clas, char item_typ, unsigned char item_da, unsigned char it_plus, unsigned int it_quant, char ident_lev, char glog [60]);
void save_id(char identy [4] [30]);
void initial(void);
void set_id(char cla, char ty, char setting);
char get_id(char cla, char ty);
void it_name(int itn, char des, char str_pass [80]);
void in_name(int inn, char des, char str_pass [80]);
int property(int pr1, int pr2, int pr3);
int mass(int pr1, int pr2);
void init_properties(void);
unsigned char check_item_knowledge(void);
char weapon_skill(char wclass, char wtype);
char damage_type(char wclass, char wtype);
char *make_name(unsigned char var1, unsigned char var2, unsigned char var3, char ncase);

/* items.cc */
void item_check(char keyin);
int add_item(int item_got);
void pickup(void);
void item_place(int item_drop_2, int x_plos, int y_plos, int quant_drop);
void drop(void);
int conv_lett(int item_drop_3);
void manage_corpses(void);

/* levels.cc */
void init_pandemonium(void);
void pandemonium_mons(void);

/* maps.cc */
char vault_main(char vgrid [81] [81], int mons_array [7], char vault_force, int many_many);

/* message.cc */
void mpr(char inf [200]);
void mesclr(void);
void more(void);
void mesclr(void);
void replay_messages(void);

/* misc.cc */
void destroy_item(unsigned int dest);
void place_chunks(int mcls, unsigned char rot_status, unsigned char chx, unsigned char chy, unsigned char ch_col);
char search_around(void);
void in_a_cloud(void);
void up_stairs(void);
void down_stairs(char remove_stairs);
void dart_trap(int trap_known, int trapped, struct bolt beam [1]);
void itrap(struct bolt beam [1], int trapped);
void cull_items(void);
void handle_traps(char trt, char trap_known);
void disarm_trap(struct dist disa [1]);
void species_ability(void);
int manage_clouds(void);
char *weird_writing(void);
void fall_into_a_pool(char place, unsigned char grype);
void new_level(void);
char *weird_colours(unsigned char coll);
char go_berserk(void);

/* monplace.cc */
int mons_place(int typed, int type_place, int px, int py, char behaviour, int hitting, unsigned char plus_seventy, int lev_mons);
int create_monster(int cls, int dur, int beha, int cr_x, int cr_y, int hitting, int zsec);
char empty_surrounds(int emx, int emy, unsigned char spc_wanted, char allow_centre, char empty [2]);

/* monstuff.cc */
char curse_an_item(char which, char power);
void monster_blink(int mn);
char random_near_space(int passed [2]);
void mons_in_cloud(int i);
int distance(int x1, int x2, int y1, int y2);
void swap_places(int swap);
void print_wounds(int wounded);
int wounded_damaged(int wound_class);
void monster(void);

/* mstruct.cc */
int mondamage(int mc, int rt);
void mon_init(char gmon_use [1000], char mcolour [1000]);
void def_letters(char letters [52] [1]);
int mon_resist_mag(int mc, char mhd);
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
char mons_see_invis(int mclass);
void define_monster(int k, struct monsters mns [MNST]);
int mons_exp_mod(int mclass);
int mons_skeleton(int mcls);
int mons_res_elec(int mclass);
int hit_points(int hit_dice, int min_hp, int rand_hp);
void mons_spell_list(unsigned char sec, int splist [6]);
//void def_letters(char letters [52] [1]);
unsigned char mons_char(int mc);
void moname(int mcl, char mench, char see_inv, char descrip, char glog [40]);
int exper_value(int mclass, char mHD, char maxhp);

char *monam(int mons_cla, int mons_e, char desc, char see_invis);


char mons_pan(int mcls); // is the monster to be found in pandemonium

int mons_flag(int mc,int bf);

/* mstuff2.cc */
void spore_goes_pop(int i);
void mons_throw(int i, struct bolt beem [1], int hand_used);
void dragon(int i, struct bolt beem [1]);
void monster_teleport(char monstel, char instan);
void mons_cast(int i, struct bolt beem [1], int spell_cast);
void mons_trap(int i);
int mons_spells(char spell_cast, int func_pass [10], char beam_name [30]);

/* newgame.cc */
char new_game(void);

/* ouch.cc */
int check_your_resists(int hurted, int flavour);

void splash_with_acid(char acid_strength);
void weapon_acid(char acid_strength);
void item_corrode(char itco);
void scrolls_burn(char burn_strength, char target_class);
void ouch(int dam, char death_source, char death_type);
void lose_level(void);
void drain_exp(void);

/* output.cc */
void print_stats(void);
char show_abilities(char ability [20], int ability_fail [20]);


/* player.cc */
int player_teleport(void);
int player_fast_run(void);
int player_speed(void);
int player_AC(void);
int player_evasion(void);
int player_shield_class(void);
int player_see_invis(void);
int player_mag_abil(void);
int burden_change(void);
int player_regen(void);
int player_sust_abil(void);
int player_res_fire(void);
int player_res_cold(void);
int player_res_poison(void);
int player_prot_life(void);
char you_resist_magic(int power);
void forget_map(char chance_forgotten);
void how_hungered(int hunge);
void gain_exp(unsigned int exp_gained);
void display_char_status(void);
void redraw_skill(char your_name [kNameLen], char clasnam [40]);
char *species_name(char speci);
char wearing_amulet(char which_am);
unsigned long exp_needed(int lev, char species);
int check_stealth(void);
void level_change(void);
int player_spec_summ(void);
int player_spec_ench(void);
int player_spec_conj(void);
int player_spec_air(void);
int player_spec_earth(void);
int player_spec_cold(void);
int player_spec_fire(void);
int player_spec_holy(void);
int player_spec_death(void);
int player_energy(void);

/* priest.cc */
char *god_name(char which_god);

void priest_spells(int func_pass [10], char religious);

char area_shift(void);

void abyss_teleport(void);

/* shopping.cc */
char in_a_shop(char shoppy, char id [4] [30]);
unsigned int item_value(unsigned char item_clas, unsigned char item_typ, unsigned char item_da, unsigned char it_plus, unsigned char it_plus2, unsigned int item_quant, char ident_lev, char id [4] [30]);
char book_rarity(char which_book);
void shop(void);

/* skills.h */
void exercise(char exsk, char deg);

void show_skills(void);

char *skill_title(char best_skill, char skill_lev, char char_class, char char_lev);

char best_skill(char min_skill, char max_skill, char excl_skill);

int species_skills(char skill, char species);

int calc_hp();
int calc_ep();

unsigned int skill_exp_needed(int lev);

char *skill_name(char which_skill);

int add_skill(int min_skill, int max_skill, int sk_tot);
void class_sk(int clsk [8] [3], int chcls);

/* skills2.h */
void show_skills(void);

char *skill_title(char best_skill, char skill_lev, char char_class, char char_lev);

char best_skill(char min_skill, char max_skill, char excl_skill);

int species_skills(char skill, char species);

int calc_hp(void);
int calc_ep(void);

unsigned int skill_exp_needed(int lev);

char *skill_name(char which_skill);

int add_skill(int min_skill, int max_skill, int sk_tot);
void class_sk(int clsk [8] [3], int chcls);

/* spell.cc */
void exercise_spell(int spell_ex, int spc);
void surge_power(int spell);
void cast_a_spell(void);
void your_spells(int spc2, int powc, char allow_fail);

/* spells.cc */
char miscast_effect(char sp_type, char mag_pow, char mag_fail, char force_effect);
char learn_a_spell(unsigned char splbook, char bitty);
int which_spellbook(void);
void read_book(char book_read);
void which_spell(void);

/* spells0.cc */
int spell_hunger(int spell_value, unsigned char spell);
int which_spell_in_book(int sbook_type, int spl);
char spellbook_contents(unsigned char plus, unsigned char type);
void spell_name(unsigned char spell, char spln [60]);
char spell_value(unsigned char spell);
int spell_type(unsigned char spell, char typy);
int spell_spec(int spell, int power);
char spell_fail(unsigned char spell);
void print_slash(char already);
char spell_list(void);

/* spells1.cc */
char spell_direction(struct dist spelld [1], struct bolt beam [1]);


void blink(void);
void random_blink(void);
void fireball(int power);
void cast_fire_storm(int powc);
void identify(char pow);
void conjure_flame(int pow);
void stinking_cloud(void);
void stinkcl(char cl_x, char cl_y, struct bolt beam [1]);

void cast_big_c(int pow, char cty);
void big_cloud(char clouds, char cl_x, char cl_y, int pow); //, struct bolt beam [1]);
void cast_lesser_healing(int mabil);
void cast_greater_healing(int mabil);
void cast_revivification(int mabil);
void cast_cure_poison(int mabil);
void purification(void);
void cast_deaths_door(int pow);
void abjuration(int pow);
void extension(int pow);
void ice_armour(int pow, char extending);
void stone_scales(int pow, char extending);
void missile_prot(int pow);
void cast_regen(int pow);
void cast_berserk(void);

void cast_swiftness(int power);
void cast_fly(int power);
void cast_teleport_control(int power);
void cast_ring_of_flames(int power);
void cast_insulation(int power);

/* spells2.cc */
int corpse_rot(int power);
char detect_traps(void);
char detect_items(int map_radius);
char detect_creatures(int map_radius);
int animate_dead(int power, int corps_beh, int corps_hit, int actual);
int animate_a_corpse(char axps, char ayps, int corps_beh, int corps_hit, char class_allowed);
int raise_corpse(int corps, char corx, char cory, int corps_beh, int corps_hit, int actual);
void cast_twisted(int power, int corps_beh, int corps_hit);
char brand_weapon(char which_brand, int power);
void manage_shock_shield(void);
int vampiric_drain(int pow);
int burn_freeze(int pow, char b_f);
int summon_elemental(int pow, unsigned char restricted_type, unsigned char unfriendly);
void summon_small_mammals(void);
void summon_scorpions(int pow);
void summon_ice_beast_etc(int pow, int ibc);
void summon_swarm(int pow);
void summon_undead(int pow);
void summon_butter(void);
void restore_str(void);
void restore_int(void);
void restore_dex(void);
void cast_refrigeration(int pow);
void summon_things(int pow);
void turn_undead(int pow); // what should I use for pow?
void holy_word(int pow);
void cast_toxic_radiance(void);

/* spells3.cc */
void entomb(void);
void you_teleport2(char allow_control);
void you_teleport(void);
void dancing_weapon(int pow);
void sublimation(int pow);
void cast_bone_shards(int power);
void cast_smiting(int pow);
void detect_curse(void);
void remove_curse(void);
void cast_selective_amnesia(void);

/* stuff.cc */
int random2(unsigned int randmax);
unsigned char get_ch(void);
char see_grid(unsigned char grx, unsigned char gry);
int magic_ability(int mag_abil, int intel);

/* view.cc */
void moname(int mcl, char mench, char see_inv, char descrip, char glog [40]);

void item(void);
void monster_grid(void);
void noisy(char loudness, char nois_x, char nois_y);

int check_awaken(int mons_aw);

void viewwindow(char draw_it);
void magic_mapping(int map_radius, int proportion);
void show_map(int spec_place [2]);
char mons_near(unsigned char monst);
void losight(unsigned int sh [19] [19], unsigned char gr [80] [70], int x_p, int y_p);
void draw_border(int bord_col, char your_name [kNameLen], char clasnam [40], char tspecies);

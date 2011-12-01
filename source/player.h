/*
 *  File:       player.cc
 *  Summary:    Player related functions.
 *  Written by: Linley Henzell
 *
 *  Change History (most recent first):
 *
 *               <1>     -/--/--        LRH             Created
 */


#ifndef PLAYER_H
#define PLAYER_H

#include "externs.h"

bool move_player_to_grid( int x, int y, bool stepped, bool allow_shift,
                          bool force );

bool player_in_mappable_area( void );
bool player_in_branch( int branch );
bool player_in_hell( void );

int         player_equip( int slot, int sub_type, bool req_id = false );
int         player_equip_ego_type( int slot, int sub_type, bool req_type_id = false );
bool        player_barehand_butcher( void );
bool        player_can_cut_meat( void );
int         player_damage_type( bool extra_unarmed = false );
int         player_damage_brand( bool extra_unarmed = false );
bool        player_weapon_wielded( void );
bool        player_tool_usable( void );
bool        player_is_bashing( void );
skill_type  player_weapon_skill( void );

bool        player_sound_mind_and_body( void );
bool        player_long_melee( void );
int         player_long_skill( const item_def &wpn );
int         player_weapon_ev_bonus( struct monsters *mon );

bool player_is_shapechanged( bool check_hands = false );
bool check_species_physiology( species_type species );
size_type player_size( int psize = PSIZE_TORSO, bool base = false );

bool is_light_armour( const item_def &item );
bool player_light_armour(void);

bool player_in_water(void);
bool player_is_swimming(void);
bool player_is_levitating(void);

bool player_under_penance(void);

bool player_access_floor(void);
bool player_control_flight( bool req_id = false );
bool player_res_corrosion( bool req_id = false );
bool player_item_conserve( bool req_id = false );
int  player_mental_clarity( bool req_id = false );
bool player_has_warding( bool req_id = false );

char *species_name( int speci, int level = -1, bool genus = false,
                    bool adj = false, bool cap = true );

bool you_resist_magic(int power);

int burden_change(void);
int carrying_capacity(void);

int player_stealth(void);
int player_armour_class(void);
int player_energy(void);
int player_evasion(void);
int player_base_stat( int stat );
int player_movement_speed(void);
int player_hunger_rate(void);
int player_mag_abil( void );
int player_magical_power( void );
int player_magic_regen(void);
int player_regen(void);

int player_prot_life(bool req_id = false);
int player_res_cold(bool req_id = false);
int player_res_acid(void);
int player_res_electricity(bool req_id = false);
int player_res_fire(bool req_id = false);
int player_res_poison(bool req_id = false);
int player_res_magic(void);

int player_shield_class(void);
bool player_can_block( struct monsters *mon = NULL );
bool player_repel_missiles(void);

int player_spec_air(void);
int player_spec_cold(void);
int player_spec_conj(void);
int player_spec_death(void);
int player_spec_earth(void);
int player_spec_ench(void);
int player_spec_fire(void);
int player_spec_holy(void);
int player_spec_poison(void);
int player_spec_summ(void);

int player_base_time_taken( void );
speed_type player_net_speed_effect( void );

int player_total_spell_levels(void);
int player_spell_levels(void);

int player_sust_abil(bool req_id = false);
int player_teleport(bool req_id = false);
int player_teleport_control(bool req_id = false);

int scan_randarts( randart_prop_type which_property, bool req_id = false );

int slaying_bonus( int which_affected, bool req_id = false );

bool player_can_smell( void );
int  player_see_invis( bool req_id = false );
bool player_monster_visible( struct monsters *mon );

unsigned long exp_needed( int lev );

void display_char_status(void);

void forget_map( int chance_forgotten );
void gain_exp( unsigned int exp_gained );

void level_change(void);

bool player_genus( int which_genus, int species = SP_UNKNOWN );
void dec_hp( int hp_loss, bool fatal );
bool enough_hp( int minimum, bool suppress_msg );
bool enough_mp( int minimum, bool suppress_msg );
void dec_mp( int mp_loss );
void inc_mp( int mp_gain, bool max_too );
void inc_hp( int hp_gain, bool max_too );
void rot_hp( int hp_loss );
void unrot_hp( int hp_recovered );
int  player_rotted( void );
void rot_mp( int mp_loss );

void inc_max_hp( int hp_gain );
void dec_max_hp( int hp_loss );

void inc_max_mp( int mp_gain );
void dec_max_mp( int mp_loss );

void set_hp( int new_amount, bool max_too );
void set_mp( int new_amount, bool max_too );

int get_species_index_by_abbrev( const char *abbrev );
int get_species_index_by_name( const char *name );
const char *get_species_abbrev( int which_species );

int get_class_index_by_abbrev( const char *abbrev );
int get_class_index_by_name( const char *name );
const char *get_class_abbrev( int which_job );
const char *get_class_name( int which_job );

void contaminate_player(int change, bool statusOnly = false);

void poison_player( int amount, bool force = false );
void reduce_poison_player( int amount );

void paralyse_player( int amount, bool quiet = false );

void confuse_player( int amount, bool resistable = true );
void reduce_confuse_player( int amount );

void stun_player( int amount );
void cut_player( int amount );

void slow_player( int amount );
void dec_slow_player();

void haste_player( int amount );
void dec_haste_player();

void disease_player( int amount );
void dec_disease_player();

void rot_player( int amount );

void fatigue_player( int amount );
bool player_is_fatigued( void );
bool player_is_exhausted( void );

bool player_has_spell( int spell );

#endif

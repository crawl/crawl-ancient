/*  File:       mon-util.h
 *  Summary:    Misc monster related functions.
 *  Written by: Linley Henzell
 *
 *  Change History (most recent first):
 *
 *               <1>     -/--/--        LRH             Created
 */


#ifndef MONUTIL_H
#define MONUTIL_H

#include "externs.h"

// ($pellbinder) (c) D.G.S.E. 1998

// ****remember***** must make an hardcopy of this sometime

#if defined(macintosh) || defined(__IBMCPP__) || defined(SOLARIS) || defined(__BCPLUSPLUS__) || defined(BSD)
#define PACKED
#else
#ifndef PACKED
#define PACKED __attribute__ ((packed))
#endif
#endif

struct mon_attack_def
{
    mon_attack_type     type;
    mon_attack_flavour  flavour;
    int                 damage;
};

struct monsterentry
{
    monster_type                mc PACKED;            // monster number

    unsigned char               showchar PACKED;
    int                         colour PACKED;
    const char                 *name PACKED;

    unsigned long               bitfields PACKED;
    unsigned long               resists PACKED;

    short                       weight PACKED;

    char                        exp_mod PACKED;

    monster_type                genus PACKED;   // "team" the monster plays for
    monster_type                species PACKED; // corpse type of the monster

    mon_holy_type               holiness PACKED;

    short                       resist_magic PACKED;

    mon_attack_def              attack[4] PACKED;
    unsigned int                hpdice[4] PACKED;

    char                        AC PACKED;
    char                        ev PACKED;
    char                        speed PACKED;
    char                        attack_cost PACKED;

    mon_spellbook_type          sec PACKED;

    corpse_effect_type          corpse_thingy PACKED;
    zombie_size_type            zombie_size PACKED;
    shout_type                  shouts PACKED;
    monster_intel_type          intel PACKED;

    mon_itemuse_type            gmon_use PACKED;
    size_type                   size PACKED;
};

void init_monsters( void );

// mons_wpn only important for dancing weapons -- bwr
const char *monam( int mons_num, int mons, bool vis, char desc,
                   int mons_wpn = NON_ITEM, bool title = false );

// these front for monam
const char *ptr_monam( struct monsters *mon, char desc, bool title = false );
const char *ptr_being_name( int mon_id, char desc, bool title = false );

int mons_class_flies( int mc );
int mons_flies( struct monsters *mon );

mon_itemuse_type  mons_itemuse(int mc);

char mons_see_invis( struct monsters *mon );

shout_type  mons_shouts(int mclass);

bool mons_is_unique( int mclass );

int exper_value( struct monsters *monster );

int hit_points(int hit_dice, int min_hp, int rand_hp);

int mons_class_hit_dice( int type );

int mons_resist_magic( struct monsters *mon );
int mons_resist_turn_undead( struct monsters *mon );

bool mons_sound_mind_and_body( struct monsters *mon, bool body_only = false );

void get_mons_attack( struct monsters *mon, int rt,
                      struct mon_attack_def &attack );
const char * get_attack_verb( mon_attack_type type );

int mons_damage( struct monsters *mon, int rt );

bool mons_light_armour( struct monsters *mon );
int apply_mons_armour( int damage, struct monsters *mon, bool energy = false );
int apply_being_armour( int def_id, int damage_size, int damage_taken, bool energy = false );

bool mons_long_melee( struct monsters *mon );
int  mons_weapon_ev_bonus( struct monsters *def, struct monsters *att = NULL );
int  being_weapon_ev_bonus( int def_id, int att_id );

int  being_evasion( int mon_id );

int mons_shield( struct monsters *mon );

int mons_shield_class( struct monsters *mon );
int being_shield_class( int mon_id );

bool mons_can_block( struct monsters *mon, struct monsters *att = NULL,
                     bool vis = false );
bool being_can_block( int def_id, int att_id, bool vis = false );

int mons_blocks_penalty( struct monsters *mon );
int being_blocks_penalty( int mon_id );

int mons_perception( struct monsters *mon );

monster_type mons_genus(int mcls);
monster_type mons_species(int mcls);
monster_type draco_subspecies( struct monsters *const mon );

corpse_effect_type mons_corpse_effect( int mclass );

bool mons_flag( struct monsters *mon, mon_instance_flag_type flag );
void set_mons_flag( struct monsters *mon, mon_instance_flag_type flag );
void clear_mons_flag( struct monsters *mon, mon_instance_flag_type flag );

bool mons_class_flag( int mc, mons_class_flags bf );
bool mons_class_resist( int mc, mon_resist_flags bf );

mon_holy_type mons_class_holiness( int mclass );
mon_holy_type mons_holiness( struct monsters *mon );

brand_type mons_weapon_brand( struct monsters *mon, int hand );

int mons_class_evasion(int mclass);

bool mons_is_mimic( int mc );
bool mons_appears_harmless( int mc );
bool mons_is_demon( int mc );
bool mons_is_humanoid( int mc );
bool mons_is_animated_undead( int mc, bool only_physical = false );

bool mons_is_summoned( struct monsters *mon );

monster_intel_type  mons_intel( int mclass );
monster_intel_type  mons_intel_type( int mclass ); //jmf: added 20mar2000

int mons_res_cold( struct monsters *mon );
int mons_res_elec( struct monsters *mon );
int mons_res_fire( struct monsters *mon );
int mons_res_acid( struct monsters *mon );
int mons_res_poison( struct monsters *mon );
int mons_res_negative_energy( struct monsters *mon );

int being_res_cold( int mid );
int being_res_elec( int mid );
int being_res_fire( int mid );
int being_res_acid( int mid );
int being_res_poison( int mid );
int being_res_negative_energy( int mid );

int being_res_damage_type( int mid, int dam_type );
int being_choose_damage_type( int mid, int dam_type );
int mons_damage_type( struct monsters *mon, const struct mon_attack_def &attack,
                      int hand_used );

void mons_submerge( struct monsters *mon );
void mons_unsubmerge( struct monsters *mon );
bool mons_is_submerged( struct monsters *mon );
bool mons_can_submerge( struct monsters *mon, int terrain = DNGN_UNSEEN );

bool mons_is_invisible( struct monsters *mon );

bool mons_monster_visible( struct monsters *mon, struct monsters *targ );
bool mons_player_visible( struct monsters *mon );
bool being_visible( int mon_id, int targ_id );

bool mons_line_of_sight( struct monsters *mon, int x, int y );

bool mons_is_evil( struct monsters *mon );
bool mons_is_unholy( struct monsters *mon );
bool mons_has_lifeforce( struct monsters *mon );

bool mons_skeleton(int mcls);
int  mons_weight(int mclass);

bool mons_is_guard( struct monsters *mon );
bool mons_stationary( int mclass );
int  mons_speed( struct monsters *mon );
int  mons_class_speed( int mclass );
int  mons_attack_cost( struct monsters *mon );

zombie_size_type  mons_zombie_size( int mclass );
size_type         mons_size( int mclass, int psize = PSIZE_TORSO );

int mons_power(int mclass);

unsigned char mons_char(int mc);

int mons_element_colour( int mc );
int mons_class_colour( int mc );

void clear_monster_enchants( int index );
void define_monster( int mid, bool polymorphing = false );

const char * moname( int mcl, bool vis, char descrip,
                     char buff[ ITEMNAME_SIZE ] );

int  mons_spell_template_index( struct monsters *mon );
void mons_load_spells( struct monsters *const mon, int book );

#if DEBUG_DIAGNOSTICS
const char *mons_spell_name( int spell );
#endif

bool mons_should_fire(struct bolt &beam);

bool ms_direct_nasty(int monspell);
bool ms_requires_tracer(int mons_spell);
bool ms_useful_fleeing_out_of_sight( struct monsters *mon, int monspell );
bool ms_waste_of_time( struct monsters *mon, int monspell );
bool ms_low_hitpoint_cast( struct monsters *mon, int monspell );
bool ms_summon_spell( int monspell );

bool mons_has_ranged_spell( struct monsters *mon );
bool mons_has_ranged_attack( struct monsters *mon );

bool mons_has_spell( struct monsters *mon, int mspell );

int  mons_has_teleport_spell( struct monsters *mon );

int  mons_has_summon_spell( struct monsters *mon );

const char *mons_pronoun( int mclass, int variant );

bool beings_aligned( int m1_id, int m2_id );
bool mons_friendly( struct monsters *m );

bool mons_add_ench( struct monsters *mon, enchant_type ench,
                    int source = MHITNOT, int duration = -1, int levels = 1 );

enchant_retval mons_inc_ench_levels( struct monsters *mon, enchant_type ench,
                                     int source, int levels );

mon_enchant_def * mons_has_ench( struct monsters *mon, enchant_type ench );

bool mons_del_ench( struct monsters *mon, enchant_type ench, bool quiet = false );

bool mons_dec_ench_ptr( struct monsters *mon, mon_enchant_def *ench_ptr,
                        int amount, bool quiet = false );

bool mons_del_ench_ptr( struct monsters *mon, mon_enchant_def *ench_ptr,
                        bool quiet = false );

int levels_to_dur( enchant_type ench, int levels = 1 );
int dur_to_levels( enchant_type ench, int dur );

bool check_mons_resist_magic( struct monsters *monster, int pow );

bool mons_can_swap( struct monsters *mon, struct monsters *targ );

bool mons_is_scared( struct monsters *mon );
bool mons_is_confused( struct monsters *monster );
bool mons_is_batty( struct monsters *monster );

bool mons_is_shapeshifter( struct monsters *monster );

void drain_monster( struct monsters *monster, bool quiet = false );



#endif

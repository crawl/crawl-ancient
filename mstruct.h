// ($pellbinder) (c) D.G.S.E. 1998

// ****remeber***** must make an hardcopy of this sometime
#define PACKED __attribute__ ((packed))
//#define PACKED
typedef char MYCHAR; // used for flags and the like
typedef unsigned char MYUCHAR; // used for flags and the like
typedef short MYSHORT; // used for flags and the like
typedef int MYINT; // used for flags and the like
// bitfield
// leaves no skeleton? ("blob" monsters?)
// if weight=0 or zombie_size=0, this is always true
#define M_NO_FLAGS 0 // clear
#define M_NO_SKELETON (1<<0)
// resistances
#define M_RES_ELEC (1<<1)
#define M_RES_POISON (1<<2)
#define M_RES_FIRE (1<<3)
#define M_RES_HELLFIRE (1<<4)
#define M_RES_COLD (1<<5)
// vulnerabilities
#define M_ED_ELEC (1<<6) // can this? - - Not as yet (LH)
#define M_ED_POISON (1<<7) // ???  - - This flag is now (2.50) set for insects (LH)
#define M_ED_FIRE (1<<8)
#define M_ED_COLD (1<<9)
// casts spells?
#define M_SPELLCASTER (1<<10)
// hovering?
#define M_FLIES (1<<11) // will crash to ground if paralysed (wings)
#define M_LEVITATE (1<<12) // not if this is set
// infrared/radar/whatever?
#define M_SEE_INVIS (1<<13)
// killing this beast only gives 10 experience (makes sense for plants/fungi)
// must do this manually
#define M_NO_EXP_GAIN (1<<14)
#define M_SPEAKS (1<<15)

// holiness
#define H_HOLY -1
#define H_NORMAL 0
#define H_UNDEAD 1
#define H_DEMONIC 2
// corpse_thingy
#define C_NOCORPSE 0
#define C_CLEAN 1
#define C_CONTAMINATED 2
#define C_POISON 3
#define C_HCL 4 // your flesh starts to rot away...
// well, HCl will burn your flesh away. Anyway, couldn't find a better name for now
#define C_MUTATE 5 // makes you mutate, of course

// zombie size
#define Z_NOZOMBIE 0 // no zombie (and no skeleton)
#define Z_SMALL 1
#define Z_BIG 2
// Note - this should be set to 0 for classed monsters, eg orc wizard

// shout
#define S_RANDOM -1
#define S_SILENT 0
#define S_SHOUT 1 //1=shout
#define S_BARK 2 //2=bark
#define S_SHOUT2 3 //3=shout twice
#define S_ROAR 4 //4=roar
#define S_SCREAM 5 //5=scream
#define S_BELLOW 6 //6=bellow (?)
#define S_SCREECH 7 //7=screech
#define S_BUZZ 8 //8=buzz
#define S_MOAN 9 //9=moan
#define S_WHINE 10 //10=irritating whine (mosquito)
// ai
// So far this only affects a) chance to see stealthy player and b) chance to
//  walk through damaging clouds (LH)
#define I_PLANT 0
#define I_INSECT 1
#define I_ANIMAL 2
#define I_NORMAL 3
#define I_HIGH 4

extern int mondatasize;
extern struct monsterentry {
        short mc PACKED; // monster number
        unsigned char showchar PACKED,colour PACKED;
        char *name/*[32]*/PACKED; // longest is 23 till now (31 is max alowed here)
        int bitfields PACKED;
        short weight PACKED;
        // experience is calculated like this:
        // ((((max_hp / 7) + 1) * (mHD * mHD) + 1) * exp_mod) / 10
        //     ^^^^^^ see below at hpdice
        //   Note that this may make draining attacks less attractive (LH)
        char exp_mod PACKED;

        short charclass PACKED; //
        char holiness PACKED; // -1=holy,0=normal,1=undead,2=very very evil

        short resist_magic PACKED; // (positive is ??)
        // max damage in a turn is total of these four?
        unsigned char damage[4] PACKED;

        // hpdice[4]: [0]=HD [1]=min_hp [2]=rand_hp [3]=add_hp
        // min hp = [0]*[1]+[3] & max hp = [0]*([1]+[2])+[3]
        // example: the Iron Golem, hpdice={15,7,4,0}
        //      15*7 < hp < 15*(7+4),
        //       105 < hp < 165
        // hp will be around 135 each time. (assuming an good random number generator)
        // !!!!!!! The system is exactly the same as before, only the way of writing changed !!!!
        unsigned char hpdice[4] PACKED; // until we have monsters with 32767 hp,this is easily possible
        char AC PACKED; // armour class
        char ev PACKED; // evasion
        char speed PACKED,speed_inc PACKED; // duh!
        short sec PACKED; // not used (250) most o/t time

        // eating the corpse: 1=clean,2=might be contaminated,3=poison,4=very bad
        char corpse_thingy PACKED;
        // 0=no zombie, 1=small zombie (z) 107, 2=_BIG_ zombie (Z) 108
        char zombie_size PACKED;
        // 0=silent, 1=shout, 2=bark (makes only sense for dogs),
        // 3=shout twice, 4=rour, 5=scream, 6=bellow (?), 7=screech,
        // 8=buzz, 9=moan, -1=random one of (0-7)
        char shouts PACKED;
        // AI things?
        char intel PACKED; // 0=none, 1=worst...4=best
        char gmon_use PACKED;
} mondata[];
// wow. this struct is only about 48 bytes, (excluding the name)


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
int exper_value(int mclass, int mHD, int maxhp);

char *monam(int mons_cla, int mons_e, char desc, char see_invis);


char mons_pan(int mcls); // is the monster to be found in pandemonium

int mons_flag(int mc,int bf);

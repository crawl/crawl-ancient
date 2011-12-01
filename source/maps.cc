/*
 *  File:       maps.cc
 *  Summary:    Functions used to create vaults.
 *  Written by: Linley Henzell
 *
 *  Change History (most recent first):
 *
 * <2>      5/20/99        BWR Added stone lining to Zot vault,
 *                             this should prevent digging?
 * <1>      -/--/--        LRH             Created
 */

#include "AppHdr.h"
#include "maps.h"

#include <string.h>
#include <stdlib.h>

#include "enum.h"
#include "globals.h"
#include "monplace.h"
#include "stuff.h"


static char vault_1(char vgrid[81][81], FixedVector<int, 7>& mons_array);
static char vault_2(char vgrid[81][81], FixedVector<int, 7>& mons_array);
static char vault_3(char vgrid[81][81], FixedVector<int, 7>& mons_array);
static char vault_4(char vgrid[81][81], FixedVector<int, 7>& mons_array);
static char vault_5(char vgrid[81][81], FixedVector<int, 7>& mons_array);
static char vault_6(char vgrid[81][81], FixedVector<int, 7>& mons_array);
static char vault_7(char vgrid[81][81], FixedVector<int, 7>& mons_array);
static char vault_8(char vgrid[81][81], FixedVector<int, 7>& mons_array);
static char vault_9(char vgrid[81][81], FixedVector<int, 7>& mons_array);
static char vault_10(char vgrid[81][81], FixedVector<int, 7>& mons_array);


static char antaeus(char vgrid[81][81], FixedVector<int, 7>& mons_array);
static char asmodeus(char vgrid[81][81], FixedVector<int, 7>& mons_array);
static char beehive(char vgrid[81][81], FixedVector<int, 7>& mons_array);
static char box_level(char vgrid[81][81], FixedVector<int, 7>& mons_array);
static char castle_dis(char vgrid[81][81], FixedVector<int, 7>& mons_array);
static char elf_hall(char vgrid[81][81], FixedVector<int, 7>& mons_array);
static char ereshkigal(char vgrid[81][81], FixedVector<int, 7>& mons_array);
static char farm_and_country(char vgrid[81][81], FixedVector<int, 7>& mons_array);
static char fort_yaktaur(char vgrid[81][81], FixedVector<int, 7>& mons_array);
static char hall_of_Zot(char vgrid[81][81], FixedVector<int, 7>& mons_array);
static char hall_of_blades(char vgrid[81][81], FixedVector<int, 7>& mons_array);
static char gloorx_vloq(char vgrid[81][81], FixedVector<int, 7>& mons_array);
//static char mollusc(char vgrid[81][81], FixedVector<int, 7>& mons_array);
static char my_map(char vgrid[81][81], FixedVector<int, 7>& mons_array);
static char mnoleg(char vgrid[81][81], FixedVector<int, 7>& mons_array);
static char cerebov(char vgrid[81][81], FixedVector<int, 7>& mons_array);
static char orc_temple(char vgrid[81][81], FixedVector<int, 7>& mons_array);
static char lom_lobon(char vgrid[81][81], FixedVector<int, 7>& mons_array);
static char slime_pit(char vgrid[81][81], FixedVector<int, 7>& mons_array);
static char snake_pit(char vgrid[81][81], FixedVector<int, 7>& mons_array);
static char swamp(char vgrid[81][81], FixedVector<int, 7>& mons_array);
static char temple(char vgrid[81][81], FixedVector<int, 7>& mons_array);
static char tomb_1(char vgrid[81][81], FixedVector<int, 7>& mons_array);
static char tomb_2(char vgrid[81][81], FixedVector<int, 7>& mons_array);
static char tomb_3(char vgrid[81][81], FixedVector<int, 7>& mons_array);
static char vaults_vault(char vgrid[81][81], FixedVector<int, 7>& mons_array);
static char vestibule_map(char vgrid[81][81], FixedVector<int, 7>& mons_array);


static char minivault_1(char vgrid[81][81], FixedVector<int, 7>& mons_array);
static char minivault_2(char vgrid[81][81], FixedVector<int, 7>& mons_array);
static char minivault_3(char vgrid[81][81], FixedVector<int, 7>& mons_array);
static char minivault_4(char vgrid[81][81], FixedVector<int, 7>& mons_array);
static char minivault_5(char vgrid[81][81], FixedVector<int, 7>& mons_array);
static char minivault_6(char vgrid[81][81], FixedVector<int, 7>& mons_array);
static char minivault_7(char vgrid[81][81], FixedVector<int, 7>& mons_array);
static char minivault_8(char vgrid[81][81], FixedVector<int, 7>& mons_array);
static char minivault_9(char vgrid[81][81], FixedVector<int, 7>& mons_array);
static char minivault_10(char vgrid[81][81], FixedVector<int, 7>& mons_array);
static char minivault_11(char vgrid[81][81], FixedVector<int, 7>& mons_array);
static char minivault_12(char vgrid[81][81], FixedVector<int, 7>& mons_array);
static char minivault_13(char vgrid[81][81], FixedVector<int, 7>& mons_array);
static char minivault_14(char vgrid[81][81], FixedVector<int, 7>& mons_array);
static char minivault_15(char vgrid[81][81], FixedVector<int, 7>& mons_array);
static char minivault_16(char vgrid[81][81], FixedVector<int, 7>& mons_array);
static char minivault_17(char vgrid[81][81], FixedVector<int, 7>& mons_array);
static char minivault_18(char vgrid[81][81], FixedVector<int, 7>& mons_array);
static char minivault_19(char vgrid[81][81], FixedVector<int, 7>& mons_array);
static char minivault_20(char vgrid[81][81], FixedVector<int, 7>& mons_array);
static char minivault_21(char vgrid[81][81], FixedVector<int, 7>& mons_array);
static char minivault_22(char vgrid[81][81], FixedVector<int, 7>& mons_array);
static char minivault_23(char vgrid[81][81], FixedVector<int, 7>& mons_array);
static char minivault_24(char vgrid[81][81], FixedVector<int, 7>& mons_array);
static char minivault_25(char vgrid[81][81], FixedVector<int, 7>& mons_array);
static char minivault_26(char vgrid[81][81], FixedVector<int, 7>& mons_array);
static char minivault_27(char vgrid[81][81], FixedVector<int, 7>& mons_array);
static char minivault_28(char vgrid[81][81], FixedVector<int, 7>& mons_array);
static char minivault_29(char vgrid[81][81], FixedVector<int, 7>& mons_array);
static char minivault_30(char vgrid[81][81], FixedVector<int, 7>& mons_array);
static char minivault_31(char vgrid[81][81], FixedVector<int, 7>& mons_array);
static char minivault_32(char vgrid[81][81], FixedVector<int, 7>& mons_array);
static char minivault_33(char vgrid[81][81], FixedVector<int, 7>& mons_array);

//jmf: originals and slim wrappers to fit into don's non-switch
static char minivault_34(char vgrid[81][81], FixedVector<int, 7>& mons_array );
static char minivault_35(char vgrid[81][81], FixedVector<int, 7>& mons_array );

static char demon_vault_1(char vgrid[81][81], FixedVector<int, 7>& mons_array);
static char demon_vault_2(char vgrid[81][81], FixedVector<int, 7>& mons_array);
static char demon_vault_3(char vgrid[81][81], FixedVector<int, 7>& mons_array);
static char demon_vault_4(char vgrid[81][81], FixedVector<int, 7>& mons_array);
static char demon_vault_5(char vgrid[81][81], FixedVector<int, 7>& mons_array);
static char demon_vault_6(char vgrid[81][81], FixedVector<int, 7>& mons_array);
static char demon_vault_7(char vgrid[81][81], FixedVector<int, 7>& mons_array);
static char demon_vault_8(char vgrid[81][81], FixedVector<int, 7>& mons_array);
static char demon_vault_9(char vgrid[81][81], FixedVector<int, 7>& mons_array);

static bool replace_one_of( char vgrid[81][81], int mx, int my,
                            char marker, char target, char nontarget );

static void replace_with_func( char vgrid[81][81], int mx, int my,
                               char marker, char (*func)( char grid ) );

// functions for replace_with_func:
static char trapped_area( char grid );
static char trapped_treasure_area( char grid );
static char plain_treasure_area( char grid );
static char good_treasure_area( char grid );
static char great_treasure_area( char grid );


/* ******************** BEGIN PUBLIC FUNCTIONS ******************* */


// remember (!!!) - if a member of the monster array isn't specified
// within a vault subroutine, assume that it holds a random monster
// -- only in cases of explicit monsters assignment have I listed
// out random monster insertion {dlb}

// make sure that vault_n, where n is a number, is a vault which can be put
// anywhere, while other vault names are for specific level ranges, etc.
char vault_main( char vgrid[81][81], FixedVector<int, 7>& mons_array,
                 int vault_force, int many_many )
{

    int which_vault = 0;
    unsigned char vx, vy;
    char (*fnc_vault) ( char[81][81], FixedVector<int, 7>& ) = 0;

    // first, fill in entirely with walls and null-terminate {dlb}:
    for (vx = 0; vx < 80; vx++)
    {
        for (vy = 0; vy < 80; vy++)
          vgrid[vx][vy] = 'x';

        vgrid[80][vx] = '\0';
        vgrid[vx][80] = '\0';
    }

    if (vault_force != VAULT_RANDOM)
        which_vault = vault_force;
    else
    {
        // next, select an appropriate vault to place {dlb}:
        for (;;)
        {
            which_vault = random2(14);

            if (which_vault == VAULT_ORC_TEMPLE)
            {
                if (many_many > 12)
                    break;
            }
            else if (which_vault == VAULT_FARM_AND_COUNTRY
                    || which_vault == VAULT_FORT_YAKTAUR)
            {
                if (many_many > 20)
                    break;
            }
            else
            {
                break;
            }
        }
    }

    // Kludges to maintain some old level based vault switching:
    if (which_vault == VAULT_ORC_TEMPLE && many_many > 23)
        which_vault = VAULT_MY_MAP;

    if (which_vault >= VAULT_MINIVAULT_6
        && which_vault <= VAULT_MINIVAULT_9
        && many_many < 15)
    {
        which_vault -= 5;
    }

#if DEBUG_DIAGNOSTICS
    mpr( MSGCH_DIAGNOSTICS, "Build: vault %d", which_vault );
#endif

    // then, determine which drawing routine to use {dlb}:
    fnc_vault = (
          (which_vault == VAULT_VAULT_1)        ? vault_1 :
          (which_vault == VAULT_VAULT_2)        ? vault_2 :
          (which_vault == VAULT_VAULT_3)        ? vault_3 :
          (which_vault == VAULT_VAULT_4)        ? vault_4 :
          (which_vault == VAULT_VAULT_5)        ? vault_5 :
          (which_vault == VAULT_VAULT_6)        ? vault_6 :
          (which_vault == VAULT_VAULT_7)        ? vault_7 :
          (which_vault == VAULT_VAULT_8)        ? vault_8 :
          (which_vault == VAULT_VAULT_9)        ? vault_9 :
          (which_vault == VAULT_ORC_TEMPLE)     ? orc_temple :
          (which_vault == VAULT_VAULT_10)       ? vault_10 :
          (which_vault == VAULT_FARM_AND_COUNTRY) ? farm_and_country :
          (which_vault == VAULT_FORT_YAKTAUR)   ? fort_yaktaur :
          (which_vault == VAULT_BOX_LEVEL)      ? box_level :
          (which_vault == VAULT_MY_MAP)         ? my_map :

          // The hell vaults:
          (which_vault == VAULT_VESTIBULE_MAP)  ? vestibule_map :
          (which_vault == VAULT_CASTLE_DIS)     ? castle_dis :
          (which_vault == VAULT_ASMODEUS)       ? asmodeus :
          (which_vault == VAULT_ANTAEUS)        ? antaeus :
          (which_vault == VAULT_ERESHKIGAL)     ? ereshkigal :

          // The pandemonium big demonlord vaults:
          (which_vault == VAULT_MNOLEG)         ? mnoleg :      // was nemelex
          (which_vault == VAULT_LOM_LOBON)      ? lom_lobon :   // was sif muna
          (which_vault == VAULT_CEREBOV)        ? cerebov :     // was okawaru
          (which_vault == VAULT_GLOORX_VLOQ)    ? gloorx_vloq : // was kikubaa
          //(which_vault ==  VAULT_MOLLUSC)     ? mollusc :

          // Branch vaults:
          (which_vault == VAULT_BEEHIVE)        ? beehive :
          (which_vault == VAULT_SLIME_PIT)      ? slime_pit :
          (which_vault == VAULT_BOTTOM_OF_VAULTS) ? vaults_vault :
          (which_vault == VAULT_HALL_OF_BLADES) ? hall_of_blades :
          (which_vault == VAULT_HALL_OF_ZOT)    ? hall_of_Zot :
          (which_vault == VAULT_TEMPLE)         ? temple :
          (which_vault == VAULT_SNAKE_PIT)      ? snake_pit :
          (which_vault == VAULT_ELF_HALL)       ? elf_hall :
          (which_vault == VAULT_TOMB_1)         ? tomb_1 :
          (which_vault == VAULT_TOMB_2)         ? tomb_2 :
          (which_vault == VAULT_TOMB_3)         ? tomb_3 :
          (which_vault == VAULT_SWAMP)          ? swamp :

          // Minivaults:
          (which_vault == VAULT_MINIVAULT_1)    ? minivault_1 :
          (which_vault == VAULT_MINIVAULT_2)    ? minivault_2 :
          (which_vault == VAULT_MINIVAULT_3)    ? minivault_3 :
          (which_vault == VAULT_MINIVAULT_4)    ? minivault_4 :
          (which_vault == VAULT_MINIVAULT_5)    ? minivault_5 :
          (which_vault == VAULT_MINIVAULT_6)    ? minivault_6 :
          (which_vault == VAULT_MINIVAULT_7)    ? minivault_7 :
          (which_vault == VAULT_MINIVAULT_8)    ? minivault_8 :
          (which_vault == VAULT_MINIVAULT_9)    ? minivault_9 :
          (which_vault == VAULT_MINIVAULT_10)   ? minivault_10 :
          (which_vault == VAULT_MINIVAULT_11)   ? minivault_11 :
          (which_vault == VAULT_MINIVAULT_12)   ? minivault_12 :
          (which_vault == VAULT_MINIVAULT_13)   ? minivault_13 :
          (which_vault == VAULT_MINIVAULT_14)   ? minivault_14 :
          (which_vault == VAULT_MINIVAULT_15)   ? minivault_15 :
          (which_vault == VAULT_MINIVAULT_16)   ? minivault_16 :
          (which_vault == VAULT_MINIVAULT_17)   ? minivault_17 :
          (which_vault == VAULT_MINIVAULT_18)   ? minivault_18 :
          (which_vault == VAULT_MINIVAULT_19)   ? minivault_19 :
          (which_vault == VAULT_MINIVAULT_20)   ? minivault_20 :
          (which_vault == VAULT_MINIVAULT_21)   ? minivault_21 :
          (which_vault == VAULT_MINIVAULT_22)   ? minivault_22 :
          (which_vault == VAULT_MINIVAULT_23)   ? minivault_23 :
          (which_vault == VAULT_MINIVAULT_24)   ? minivault_24 :
          (which_vault == VAULT_MINIVAULT_25)   ? minivault_25 :
          (which_vault == VAULT_MINIVAULT_26)   ? minivault_26 :
          (which_vault == VAULT_MINIVAULT_27)   ? minivault_27 :
          (which_vault == VAULT_MINIVAULT_28)   ? minivault_28 :
          (which_vault == VAULT_MINIVAULT_29)   ? minivault_29 :
          (which_vault == VAULT_MINIVAULT_30)   ? minivault_30 :
          (which_vault == VAULT_MINIVAULT_31)   ? minivault_31 :
          (which_vault == VAULT_MINIVAULT_32)   ? minivault_32 :
          (which_vault == VAULT_MINIVAULT_33)   ? minivault_33 :
          (which_vault == VAULT_MINIVAULT_34)   ? minivault_34 :
          (which_vault == VAULT_MINIVAULT_35)   ? minivault_35 :

          // Random demolord minivaults:
          (which_vault == VAULT_RAND_DEMON_1)   ? demon_vault_1 :
          (which_vault == VAULT_RAND_DEMON_2)   ? demon_vault_2 :
          (which_vault == VAULT_RAND_DEMON_3)   ? demon_vault_3 :
          (which_vault == VAULT_RAND_DEMON_4)   ? demon_vault_4 :
          (which_vault == VAULT_RAND_DEMON_5)   ? demon_vault_5 :
          (which_vault == VAULT_RAND_DEMON_6)   ? demon_vault_6 :
          (which_vault == VAULT_RAND_DEMON_7)   ? demon_vault_7 :
          (which_vault == VAULT_RAND_DEMON_8)   ? demon_vault_8 :
          (which_vault == VAULT_RAND_DEMON_9)   ? demon_vault_9 : 0);    // yep, NULL -- original behaviour {dlb}

    // NB - a return value of zero is not handled well by dungeon.cc (but there it is) 10mar2000 {dlb}
    return ((fnc_vault == 0) ? 0 : fnc_vault( vgrid, mons_array ));
}          // end vault_main()

/* ********************* END PUBLIC FUNCTIONS ******************** */

/*
   key:
   x - DNGN_ROCK_WALL
   X - DNGN_PERMAROCK_WALL -> should always be undiggable! -- bwr
   c - DNGN_STONE_WALL
   v - DNGN_METAL_WALL
   b - DNGN_GREEN_CRYSTAL_WALL
   a - DNGN_WAX_WALL
   . - DNGN_FLOOR
   + - DNGN_CLOSED_DOOR
   = - DNGN_SECRET_DOOR
   @ - entry point - must be on outside and on a particular side - see templates
   s - shallow water
   w - water
   l - lava
   >< - extra stairs - you can leave level by these but will never be placed on them from another level
   }{ - stairs 82/86 - You must be able to reach these from each other
   )( - stairs 83/87
   ][ - stairs 84/88
   I - orcish idol (does nothing)
   ^ - random trap

   A - Vestibule gateway (opened by Horn). Can also be put on other levels for colour, where it won't do anything.
   B - Altar. These are assigned specific types (eg of Zin etc) in dungeon.cc, in order.
   C - Random Altar.
   F - Typically a Granite Statue, but may be Orange or Silver (1 in 100)
   G - Granite statue (does nothing)
   H - orange crystal statue (attacks mind)
   S - Silver statue (summons demons). Avoid using (rare).
   T - Water fountain
   U - Magic fountain
   V - Permenantly dry fountain

   Statues can't be walked over and are only destroyed by disintegration

   $ - gold
   % - normal item
   * - higher level item (good)
   | - acquirement-level item (almost guaranteed excellent)
   O - place an appropriate rune here
   R - honeycomb (2/3) or royal jelly (1/3)
   Z - the Orb of Zot

   0 - normal monster
   9 - +5 depth monster
   8 - (+2) * 2 depth monster (aargh!). Can get golden dragons and titans this way.
   1-7 - monster array monster
   used to allocate specific monsters for a vault.
   is filled with RANDOM_MONSTER if monster not specified

   note that a lot of the vaults are in there mainly to add some interest to the
   scenery, and are not the lethal treasure-fests you find in Angband
   (not that there's anything wrong with that)

   Guidelines for creating new vault maps:

   Basically you can just let your creativity run wild. You do not have
   to place all of the stairs unless the level is full screen, in which
   case you must place all except the extra stairs (> and <). The <> stairs
   can be put anywhere and in any quantities but do not have to be there. Any
   of the other stairs which are not present in the vault will be randomly
   placed outside it. Also generally try to avoid rooms with no exit.

   You can use the templates below to build vaults, and remember to return the
   same number (this number is used in builder2.cc to determine where on the map
   the vault is located). The entry point '@' must be present (except
   full-screen vaults where it must not) and be on the same side of the vault
   as it is on the template, but can be anywhere along that side.

   You'll have to tell me the level range in which you want the vault to appear,
   so that I can code it into the vault management function. Unless you've
   placed specific monster types this will probably be unnecessary.

   I think that's all. Have fun!

   ps - remember to add one to the monster array value when placing monsters
        on each map (it is 1-7, not 0-6) {dlb}
 */

static char vault_1(char vgrid[81][81], FixedVector<int, 7>& mons_array)
{     // my first vault
    strcpy(vgrid[0],  "xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx");
    strcpy(vgrid[1],  "xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx");
    strcpy(vgrid[2],  "xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx");
    strcpy(vgrid[3],  "xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx");
    strcpy(vgrid[4],  "xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx");
    strcpy(vgrid[5],  "xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx");
    strcpy(vgrid[6],  "xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx");
    strcpy(vgrid[7],  "xxxxxxxxxxxx....x........x........x.................................xxxxxxxxxxxx");
    strcpy(vgrid[8],  "xxxxxxxxxx|=8...x........+........x......x....x1...x2...x2...x3...x...xxxxxxxxxx");
    strcpy(vgrid[9],  "xxxxxxxxxx|x....x........x........x....................................xxxxxxxxx");
    strcpy(vgrid[10], "xxxxxxxxxxxxxxxx+xxx+xxxxxxxxxxxxxx..................................xxxxxxxxxxx");
    strcpy(vgrid[11], "xxxxxxxxx.......x.................+...................................8xxxxxxxxx");
    strcpy(vgrid[12], "xxxxxxxxx.......x.................x..................................xxxxxxxxxxx");
    strcpy(vgrid[13], "xxxxxxxxx.......+........3........xx+xx................................xxxxxxxxx");
    strcpy(vgrid[14], "xxxxxxxxx.......x.................x...x..x....x1...x2...x2...x3...x...xxxxxxxxxx");
    strcpy(vgrid[15], "xxxxxxxxx.......x.................x...x.............................xxxxxxxxxxxx");
    strcpy(vgrid[16], "xxxxxxxxxx+xxxxxxxxxxxxxxxxxxxxxxxx...xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx");
    strcpy(vgrid[17], "xxxxxxxxx.........................x.S.x...xxxxxx..................|||||xxxxxxxxx");
    strcpy(vgrid[18], "xxxxxxxxx....xxxxxxxxxxxxxxxxxx...x...x......xxxxxx..................||xxxxxxxxx");
    strcpy(vgrid[19], "xxxxxxxxx....x...$$$$x****.999x...x...x.........xxxxxx.................xxxxxxxxx");
    strcpy(vgrid[20], "xxxxxxxxx....+...$$$$x****....x...x...+............xxxxxx.........8....xxxxxxxxx");
    strcpy(vgrid[21], "xxxxxxxxx....x...$$$$x****....+...x...x...............xxxxxx...........xxxxxxxxx");
    strcpy(vgrid[22], "xxxxxxxxx....x...$$$$x****....x...x999x..................xxxxxx........xxxxxxxxx");
    strcpy(vgrid[23], "xxxxxxxxx....xxxxxxxxxxxxxxxxxx...x...xxx...................xxxxxx.....xxxxxxxxx");
    strcpy(vgrid[24], "xxxxxxxxx.........................x...xxxxxx...................xxxxxx..xxxxxxxxx");
    strcpy(vgrid[25], "xxxxxxxxxxxxx+xxxxxxxx+xxxxxxx+xxxx...xxxxxx+xxxxxxxx+xxxxxxxx+xxxxxxx=xxxxxxxxx");
    strcpy(vgrid[26], "xxxxxxxxx.........x.......x.......x...x.........x........x.............xxxxxxxxx");
    strcpy(vgrid[27], "xxxxxxxxx.........x.......x.......x...x.........x........x.............xxxxxxxxx");
    strcpy(vgrid[28], "xxxxxxxxx.........x.......x.......x...x.........x........x.............xxxxxxxxx");
    strcpy(vgrid[29], "xxxxxxxxx....1....x...2...x...3...x...x....3....x....2...x......1......xxxxxxxxx");
    strcpy(vgrid[30], "xxxxxxxxx.........x.......x.......x...x.........x........x.............xxxxxxxxx");
    strcpy(vgrid[31], "xxxxxxxxx.........x.......x.......x...x.........x........x.............xxxxxxxxx");
    strcpy(vgrid[32], "xxxxxxxxx.........x.......x.......x...x.........x........x.............xxxxxxxxx");
    strcpy(vgrid[33], "xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx...xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx");
    strcpy(vgrid[34], "xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx...xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx");
    strcpy(vgrid[35], "xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx@xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx");

    mons_array[0] = MONS_SHAPESHIFTER;
    mons_array[1] = MONS_SHAPESHIFTER;
    mons_array[2] = MONS_GLOWING_SHAPESHIFTER;

    return MAP_NORTH;
}


static char vault_2(char vgrid[81][81], FixedVector<int, 7>& mons_array)
{     // cell vault
    UNUSED( mons_array );

    strcpy(vgrid[0],  "xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx");
    strcpy(vgrid[1],  "xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx");
    strcpy(vgrid[2],  "xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx");
    strcpy(vgrid[3],  "xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx");
    strcpy(vgrid[4],  "xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx");
    strcpy(vgrid[5],  "xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx");
    strcpy(vgrid[6],  "xxxxxxxxcccccccccccccccccccccccccccccccc");
    strcpy(vgrid[7],  "xxxxxxxxccw......^......w......^......wc");
    strcpy(vgrid[8],  "xxxxxxxxcc.ccccccccccccc.ccccccccccccc.c");
    strcpy(vgrid[9],  "xxxxxxxxcc.c....c.c....c.c....c.c....c.c");
    strcpy(vgrid[10], "xxxxxxxxcc.c.8..+.c....c.c....+.c..9.c.c");
    strcpy(vgrid[11], "xxxxxxxxcc.c....c.+..9.c.c.9..c.+....c.c");
    strcpy(vgrid[12], "xxxxxxxxcc.c....c.c....c.c....c.c....c.c");
    strcpy(vgrid[13], "xxxxxxxxcc.cccccc.cccccc.cccccc.cccccc.c");
    strcpy(vgrid[14], "xxxxxxxxcc^c....c.c....c.c....c.c....c.c");
    strcpy(vgrid[15], "xxxxxxxxcc.c....c.c....c.c....+.c....c.c");
    strcpy(vgrid[16], "xxxxxxxxcc.c8...+.+..8.c.c.8..c.+....c.c");
    strcpy(vgrid[17], "xxxxxxxxcc.c....c.c....c.c....c.c....c.c");
    strcpy(vgrid[18], "xxxxxxxxcc.cccccc.cccccc.cccccc.cccccc.c");
    strcpy(vgrid[19], "xxxxxxxxcc.c....c.c....c.c....c.c....c.c");
    strcpy(vgrid[20], "xxxxxxxxcc.c....+.c....c.c.0..c.c....c.c");
    strcpy(vgrid[21], "xxxxxxxxcc.c..9.c.+.8..c^c....+.+.0..c.c");
    strcpy(vgrid[22], "xxxxxxxxcc.c....c.c....c.c....c.c....c.c");
    strcpy(vgrid[23], "xxxxxxxxcc.cccccc.cccccc.cccccc.cccccc.c");
    strcpy(vgrid[24], "xxxxxxxxcc.c....c.c....c.c....c.c....c.c");
    strcpy(vgrid[25], "xxxxxxxxcc.c.0..+.+.0..c.c....+.+....c.c");
    strcpy(vgrid[26], "xxxxxxxxcc.c....c.c....c.c.0..c.c.8..c.c");
    strcpy(vgrid[27], "xxxxxxxxcc.cccccc.c....c.c....c.cccccc.c");
    strcpy(vgrid[28], "xxxxxxxxcc.c....c.cccccc.cccccc.c....c^c");
    strcpy(vgrid[29], "xxxxxxxxcc.c....c.c....c.c..9.+.+....c.c");
    strcpy(vgrid[30], "xxxxxxxxcc.c.0..+.+....c.c9...c.c.0..c.c");
    strcpy(vgrid[31], "xxxxxxxxcc.c....c.c.8..c.c....c.c....c.c");
    strcpy(vgrid[32], "xxxxxxxxcc.cccccc^cccccc.cccccc^cccccc.c");
    strcpy(vgrid[33], "xxxxxxxxccw.......Twwwwc.cwwwwT.......wc");
    strcpy(vgrid[34], "xxxxxxxxcccccccccccccccc.ccccccccccccccc");
    strcpy(vgrid[35], "xxxxxxxxxxxxxxxxxxxxxxxc@cxxxxxxxxxxxxxx");

    return MAP_NORTHWEST;
}


static char vault_3(char vgrid[81][81], FixedVector<int, 7>& mons_array)
{     // little maze vault
    UNUSED( mons_array );

    for (unsigned char i = 0; i < 81; i++)
        strcpy(vgrid[i], "xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx");

    strcat(vgrid[0],  "xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx");
    strcat(vgrid[1],  "xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx");
    strcat(vgrid[2],  "xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx");
    strcat(vgrid[3],  "xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx");
    strcat(vgrid[4],  "xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx");
    strcat(vgrid[5],  "xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx");
    strcat(vgrid[6],  "xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx");
    strcat(vgrid[7],  "xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx");
    strcat(vgrid[8],  "xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx");
    strcat(vgrid[9],  "x900x..............x..........xxxxxxxxxx");
    strcat(vgrid[10], "x999x.xxxxxxxxxxxx.x.xxxxxxxx.xxxxxxxxxx");
    strcat(vgrid[11], "x000x.x............x.x......x.xxxxxxxxxx");
    strcat(vgrid[12], "xx.xx.xxxxxxxxxxxxxx.x.xxxx.x.xxxxxxxxxx");
    strcat(vgrid[13], "xx.x..............xx.x.88|x.x.xxxxxxxxxx");
    strcat(vgrid[14], "xx.x.x.xxxxxxxxxx.xx.xxxxxx.x.xxxxxxxxxx");
    strcat(vgrid[15], "xx.x.x.x........x...........x.xxxxxxxxxx");
    strcat(vgrid[16], "xx.x.x.x.xxxxxx.xxxxxxxxxxxxx.xxxxxxxxxx");
    strcat(vgrid[17], "xx.xxx.x.x$$$$x...............xxxxxxxxxx");
    strcat(vgrid[18], "xx.....x.x$$$$x.xxxxxxxxxxxxxxxxxxxxxxxx");
    strcat(vgrid[19], "xxxxxxxx.x$$$$x...............xxxxxxxxxx");
    strcat(vgrid[20], "x........x$$$$x.xxxxxxxxxxxxx.xxxxxxxxxx");
    strcat(vgrid[21], "x.xxxxxx.xxxx.x.............x.xxxxxxxxxx");
    strcat(vgrid[22], "x.xxxxxx.xxxx.xxxxxxxxxxxxx.x.xxxxxxxxxx");
    strcat(vgrid[23], "x.x.......xxx.x...........x.x.xxxxxxxxxx");
    strcat(vgrid[24], "x.x.xxxxx.....x.x.xxxxx...x.x.xxxxxxxxxx");
    strcat(vgrid[25], "x.x.x999xxxxxxx.x.x***x...x.x.xxxxxxxxxx");
    strcat(vgrid[26], "x.x.x889........x.x|||xxxxx.x.xxxxxxxxxx");
    strcat(vgrid[27], "x.x.x899x.xxxxx.x.x***xxxxx.x.xxxxxxxxxx");
    strcat(vgrid[28], "x.x.xxxxx.xxxxx.x.xx.xxxxxx.x.xxxxxxxxxx");
    strcat(vgrid[29], "x.x..........xx.x.xx........x.xxxxxxxxxx");
    strcat(vgrid[30], "x.xxxxxxx.xx.xx.x.xxxxx.xxxxx.xxxxxxxxxx");
    strcat(vgrid[31], "x.xxx000x.xx.xx.x.x$$$x.xxxxx.xxxxxxxxxx");
    strcat(vgrid[32], "x|||x000x.x$$$x.x.x$$$x%%x%%%.xxxxxxxxxx");
    strcat(vgrid[33], "x|||x000..x$8$x.x.x$$$x%%x%8%xxxxxxxxxxx");
    strcat(vgrid[34], "x|||xxxxxxx$$$x.x..$$$xxxx%%%xxxxxxxxxxx");
    strcat(vgrid[35], "xxxxxxxxxxxxxxx@xxxxxxxxxxxxxxxxxxxxxxxx");

    return MAP_NORTHEAST;
}


static char vault_4(char vgrid[81][81], FixedVector<int, 7>& mons_array)
{     // thingy vault
    UNUSED( mons_array );

    for (unsigned char i = 0; i < 81; i++)
        strcpy(vgrid[i], "xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx");

    strcpy(vgrid[36], "xxxxxxxxxxxxxxxxxxxxxxxxx@xxxxxxxxxxxxxx");
    strcpy(vgrid[37], "xxxxxxxxxxxxxxxxxxxxxxxxx^xxxxxxxxxxxxxx");
    strcpy(vgrid[38], "xxxxxxxxxxxxxxxxxxxxxxxx...xxxxxxxxxxxxx");
    strcpy(vgrid[39], "xxxxxxxxxxxxxxxxxxxxxxx.....xxxxxxxxxxxx");
    strcpy(vgrid[40], "xxxxxxxxxxxxxxxxxxxxxxxx...xxxxxxxxxxxxx");
    strcpy(vgrid[41], "xxxxxxxxxxxxxxxxxxxxxxxx...xxxxxxxxxxxxx");
    strcpy(vgrid[42], "xxxxxxxxxxxxxxxxxxxxxxx.....xxxxxxxxxxxx");
    strcpy(vgrid[43], "xxxxxxxxxxxxxxxxxxxxx.........xxxxxxxxxx");
    strcpy(vgrid[44], "xxxxxxxxxxxxxxxxx......0...0......xxxxxx");
    strcpy(vgrid[45], "xxxxxxxxxxxxxx.......................xxx");
    strcpy(vgrid[46], "xxxxxxxxxxxxxx.........0...0.........xxx");
    strcpy(vgrid[47], "xxxxxxxxxxxxx8......0.........0......8xx");
    strcpy(vgrid[48], "xxxxxxxxxxxxxx.........0...0.........xxx");
    strcpy(vgrid[49], "xxxxxxxxxxxxxx.......................xxx");
    strcpy(vgrid[50], "xxxxxxxxxxxxxxx........0...0........xxxx");
    strcpy(vgrid[51], "xxxxxxxxxxxxxxxxxxxx...........xxxxxxxxx");
    strcpy(vgrid[52], "xxxxxxxxxxxxxxxxxxxxxxxx...xxxxxxxxxxxxx");
    strcpy(vgrid[53], "xxxxxxxxxxxxxxxxxxxxxxxx...xxxxxxxxxxxxx");
    strcpy(vgrid[54], "xxxxxxxxxxxxxxxxxxxxxxxxx=xxxxxxxxxxxxxx");
    strcpy(vgrid[55], "xxxxxxxxxxxxxxxxxxxxxxxxx.xxxxxxxxxxxxxx");
    strcpy(vgrid[56], "xxxxxxxxxxxxxxxxxxxxxxxxx.xxxxxxxxxxxxxx");
    strcpy(vgrid[57], "xxxxxxxxxxxxxxxxxxxxxxx.....xxxxxxxxxxxx");
    strcpy(vgrid[58], "xxxxxxxxxxxxxxxxxx...............xxxxxxx");
    strcpy(vgrid[59], "xxxxxxxxxxxxxxxx8.................8xxxxx");
    strcpy(vgrid[60], "xxxxxxxxxxxxxxxxxxx.............xxxxxxxx");
    strcpy(vgrid[61], "xxxxxxxxxxxxxxxxxxxxxxxx999xxxxxxxxxxxxx");
    strcpy(vgrid[62], "xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx");
    strcpy(vgrid[63], "xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx");
    strcpy(vgrid[64], "xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx");
    strcpy(vgrid[65], "xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx");
    strcpy(vgrid[66], "xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx");
    strcpy(vgrid[67], "xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx");
    strcpy(vgrid[68], "xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx");
    strcpy(vgrid[69], "xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx");

    return MAP_SOUTHWEST;
}

static char vault_5(char vgrid[81][81], FixedVector<int, 7>& mons_array)
{     // hourglass vault
    UNUSED( mons_array );

    for (unsigned char i = 0; i < 81; i++)
        strcpy(vgrid[i], "xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx");

    strcat(vgrid[36], "xxxxxxxxxxxxxx@xxxxxxxxxxxxxxxxxxxxxxxxx");
    strcat(vgrid[37], "xxxxxx.................xxxxxxxxxxxxxxxxx");
    strcat(vgrid[38], "xxxxx...................xxxxxxxxxxxxxxxx");
    strcat(vgrid[39], "xxxxx...................xxxxxxxxxxxxxxxx");
    strcat(vgrid[40], "xxxxxx.................xxxxxxxxxxxxxxxxx");
    strcat(vgrid[41], "xxxxxx.................xxxxxxxxxxxxxxxxx");
    strcat(vgrid[42], "xxxxxx.................xxxxxxxxxxxxxxxxx");
    strcat(vgrid[43], "xxxxxxx...............xxxxxxxxxxxxxxxxxx");
    strcat(vgrid[44], "xxxxxxx...............xxxxxxxxxxxxxxxxxx");
    strcat(vgrid[45], "xxxxxxxx.............xxxxxxxxxxxxxxxxxxx");
    strcat(vgrid[46], "xxxxxxxxx.....8.....xxxxxxxxxxxxxxxxxxxx");
    strcat(vgrid[47], "xxxxxxxxxx...999...xxxxxxxxxxxxxxxxxxxxx");
    strcat(vgrid[48], "xxxxxxxxxxxx00000xxxxxxxxxxxxxxxxxxxxxxx");
    strcat(vgrid[49], "xxxxxxxxxxxxxx=xxxxxxxxxxxxxxxxxxxxxxxxx");
    strcat(vgrid[50], "xxxxxxxxxxxx.....xxxxxxxxxxxxxxxxxxxxxxx");
    strcat(vgrid[51], "xxxxxxxxxx.........xxxxxxxxxxxxxxxxxxxxx");
    strcat(vgrid[52], "xxxxxxxxx....999....xxxxxxxxxxxxxxxxxxxx");
    strcat(vgrid[53], "xxxxxxxx.............xxxxxxxxxxxxxxxxxxx");
    strcat(vgrid[54], "xxxxxxx...............xxxxxxxxxxxxxxxxxx");
    strcat(vgrid[55], "xxxxxxx...............xxxxxxxxxxxxxxxxxx");
    strcat(vgrid[56], "xxxxxx.................xxxxxxxxxxxxxxxxx");
    strcat(vgrid[57], "xxxxxx...8.........8...xxxxxxxxxxxxxxxxx");
    strcat(vgrid[58], "xxxxxx.................xxxxxxxxxxxxxxxxx");
    strcat(vgrid[59], "xxxxx.........~.........xxxxxxxxxxxxxxxx");
    strcat(vgrid[60], "xxxxx.......~~~~~.......xxxxxxxxxxxxxxxx");
    strcat(vgrid[61], "xxxxxx~~~~~~~~~~~~~~~~~xxxxxxxxxxxxxxxxx");
    strcat(vgrid[62], "xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx");
    strcat(vgrid[63], "xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx");
    strcat(vgrid[64], "xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx");
    strcat(vgrid[65], "xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx");
    strcat(vgrid[66], "xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx");
    strcat(vgrid[67], "xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx");
    strcat(vgrid[68], "xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx");
    strcat(vgrid[69], "xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx");

    replace_with_func( vgrid, 70, 62, '~', good_treasure_area );

    return MAP_SOUTHEAST;
}


static char vault_6(char vgrid[81][81], FixedVector<int, 7>& mons_array)
{     // a more Angbandy vault
    UNUSED( mons_array );

    for (unsigned char i = 0; i < 81; i++)
        strcpy(vgrid[i], "xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx");

    strcat(vgrid[0],  "xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx");
    strcat(vgrid[1],  "xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx");
    strcat(vgrid[2],  "xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx");
    strcat(vgrid[3],  "xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx");
    strcat(vgrid[4],  "xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx");
    strcat(vgrid[5],  "xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx");
    strcat(vgrid[6],  "ccccccccccccccccccccccccccccccccxxxxxxxx");
    strcat(vgrid[7],  "c~~~~~~~cc..9...cc.=8c0c*c.c*c8cxxxxxxxx");
    strcat(vgrid[8],  "c~~~~~~cc..cc..cc..cc0c.c.c.c8ccxxxxxxxx");
    strcat(vgrid[9],  "c~~~~~cc..cc..cc..cc.c$c.c.c8c.cxxxxxxxx");
    strcat(vgrid[10], "c~~~~cc9.cc..cc8.cc|c.c|c.c*c0ccxxxxxxxx");
    strcat(vgrid[11], "c~~~cc..cc..cc..cc.c.c.c.c.c.c$cxxxxxxxx");
    strcat(vgrid[12], "c~~cc..cc8.cc..cc.c*c.c.c.c.c.ccxxxxxxxx");
    strcat(vgrid[13], "c^cc9.cc..cc..cc.c.c.c.c*c.c.c.cxxxxxxxx");
    strcat(vgrid[14], "c=c..cc..cc^.cc.c$c.c.c.c.c.c*ccxxxxxxxx");
    strcat(vgrid[15], "c^^.cc..cc^^cc.c.c.c9c$c.c.c.c9cxxxxxxxx");
    strcat(vgrid[16], "c..cc..cc^^cc$c.c.c*c.c.c.c9c9ccxxxxxxxx");
    strcat(vgrid[17], "c.cc..cc..cc.c.c|c.c.c.c.c$c.c9cxxxxxxxx");
    strcat(vgrid[18], "ccc^^cc..cc.c.c.c.c.c.c.c.c.cc=cxxxxxxxx");
    strcat(vgrid[19], "cc.^cc..cc.c*c.c.c.c.c.c$c.cc^^cxxxxxxxx");
    strcat(vgrid[20], "c0.cc..cc.c.c.c.c8c.c*c.c.cc0.ccxxxxxxxx");
    strcat(vgrid[21], "c^cc..cc*c.c.c.c.c$c.c.c.cc..cccxxxxxxxx");
    strcat(vgrid[22], "c^c..cc.c.c9c.c.c.c.c.c.cc..cc.cxxxxxxxx");
    strcat(vgrid[23], "c0..cc$c.c.c*c0c.c.c.c.cc..cc.0cxxxxxxxx");
    strcat(vgrid[24], "c..cc.c.c9c.c.c.c$c.c.cc.9cc...cxxxxxxxx");
    strcat(vgrid[25], "c.cc9c.c.c.c.c.c.c.c.cc..cc^^c^cxxxxxxxx");
    strcat(vgrid[26], "ccc.c.c$c.c.c.c.c.c$cc..cc.^cc^cxxxxxxxx");
    strcat(vgrid[27], "cc$c.c.c.c.c$c.c0c.cc..cc..cc..cxxxxxxxx");
    strcat(vgrid[28], "c.c.c.c.c.c.c.c.c.cc9.cc..cc..ccxxxxxxxx");
    strcat(vgrid[29], "cc.c8c.c.c$c.c.c.cc..cc..cc0.cccxxxxxxxx");
    strcat(vgrid[30], "c.c$c.c$c0c.c.c.cc..cc..cc..cc$cxxxxxxxx");
    strcat(vgrid[31], "cc.c.c.c.c.c*c.cc..cc..cc..cc$$cxxxxxxxx");
    strcat(vgrid[32], "c.c.c.c.c.c.c.cc..cc0.cc^^cc$$$cxxxxxxxx");
    strcat(vgrid[33], "cc.c.c.c.c.c$cc..cc^.cc.^cc$$$$cxxxxxxxx");
    strcat(vgrid[34], "c.c.c.c.c.c.cc.8.^^^cc..^=^$$$$cxxxxxxxx");
    strcat(vgrid[35], "cccc@cccccccccccccccccccccccccccxxxxxxxx");

    replace_with_func( vgrid, 70, 35, '~', good_treasure_area );
    replace_with_func( vgrid, 70, 35, '^', trapped_area );

    return MAP_NORTHEAST;
}

static char vault_7(char vgrid[81][81], FixedVector<int, 7>& mons_array)
{     // four-leaf vault
    UNUSED( mons_array );

    strcpy(vgrid[0],  "xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx");
    strcpy(vgrid[1],  "xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx");
    strcpy(vgrid[2],  "xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx");
    strcpy(vgrid[3],  "xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx");
    strcpy(vgrid[4],  "xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx");
    strcpy(vgrid[5],  "xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx");
    strcpy(vgrid[6],  "xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx");
    strcpy(vgrid[7],  "xxxxxxxxxxxxx.......^^^..^^^.......xxxxx");
    strcpy(vgrid[8],  "xxxxxxxxxxxx...cccccccc..cccccccc...xxxx");
    strcpy(vgrid[9],  "xxxxxxxxxxx^^.ccccccccc..ccccccccc.^^xxx");
    strcpy(vgrid[10], "xxxxxxxxxx.^^cc~~....cc..cc....~~cc^^.xx");
    strcpy(vgrid[11], "xxxxxxxxx...cc~~~....cc..cc....~~~cc...x");
    strcpy(vgrid[12], "xxxxxxxxx..cc~~~~....cc^^cc....~~~~cc..x");
    strcpy(vgrid[13], "xxxxxxxxx..cc~~~.00..cc^^cc..00.~~~cc..x");
    strcpy(vgrid[14], "xxxxxxxxx..cc....09..cc..cc..90....cc..x");
    strcpy(vgrid[15], "xxxxxxxxx..cc......=cc....cc=......cc..x");
    strcpy(vgrid[16], "xxxxxxxxx..cc......c^^....^^c......cc..x");
    strcpy(vgrid[17], "xxxxxxxxx..ccccccccc^^....^^ccccccccc..x");
    strcpy(vgrid[18], "xxxxxxxxx..cccccccc..........cccccccc..x");
    strcpy(vgrid[19], "xxxxxxxxx.....^^.......TT.......^^.....x");
    strcpy(vgrid[20], "xxxxxxxxx.....^^.......TT.......^^.....x");
    strcpy(vgrid[21], "xxxxxxxxx..cccccccc..........cccccccc..x");
    strcpy(vgrid[22], "xxxxxxxxx..ccccccccc^^....^^ccccccccx..x");
    strcpy(vgrid[23], "xxxxxxxxx..cc......c^^....^^c......xx..x");
    strcpy(vgrid[24], "xxxxxxxxx..cc......=cc....cc=......xx..x");
    strcpy(vgrid[25], "xxxxxxxxx..cc....09..cc..cc..90....xx..x");
    strcpy(vgrid[26], "xxxxxxxxx..cc~~~.00..cc^^cc..00.~~~xx..x");
    strcpy(vgrid[27], "xxxxxxxxx..cc~~~~....cc^^cc....~~~~xx..x");
    strcpy(vgrid[28], "xxxxxxxxx...cc~~~....cc..cc....~~~xx...x");
    strcpy(vgrid[29], "xxxxxxxxxx...cc~~....cc..cc....~~xx...xx");
    strcpy(vgrid[30], "xxxxxxxxxxx...ccccccccc..ccccccccx...xxx");
    strcpy(vgrid[31], "xxxxxxxxxxxx...cccccccc..cccccccc...xxxx");
    strcpy(vgrid[32], "xxxxxxxxxxxxx.^^^....^^^^^^...^^^..xxxxx");
    strcpy(vgrid[33], "xxxxxxxxxxxxxxxxxxxxxxx^^xxxxxxxxxxxxxxx");
    strcpy(vgrid[34], "xxxxxxxxxxxxxxxxxxxxxxx==xxxxxxxxxxxxxxx");
    strcpy(vgrid[35], "xxxxxxxxxxxxxxxxxxxxxxx@.xxxxxxxxxxxxxxx");

    replace_with_func( vgrid, 40, 35, '~', good_treasure_area );
    replace_with_func( vgrid, 40, 35, '^', trapped_area );

    return MAP_NORTHWEST;
}

static char vault_8(char vgrid[81][81], FixedVector<int, 7>& mons_array)
{     // cross vault
    UNUSED( mons_array );

    strcpy(vgrid[0], "xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx");
    strcpy(vgrid[1], "xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx");
    strcpy(vgrid[2], "xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx");
    strcpy(vgrid[3], "xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx");
    strcpy(vgrid[4], "xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx");
    strcpy(vgrid[5], "xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx");
    strcpy(vgrid[6], "xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx");
    strcpy(vgrid[7], "xxxxxxxxxxxxxxxxxx............xxxxxxxxxx");
    strcpy(vgrid[8], "xxxxxxxxxxxxxxx..................xxxxxxx");
    strcpy(vgrid[9], "xxxxxxxxxxxxx......................xxxxx");
    strcpy(vgrid[10], "xxxxxxxxxxxx..........wsww..........xxxx");
    strcpy(vgrid[11], "xxxxxxxxxxx........wwwwwswwww........xxx");
    strcpy(vgrid[12], "xxxxxxxxxxx......wwwvvv==vvvwww......xxx");
    strcpy(vgrid[13], "xxxxxxxxxx......wwwwv9....9vwwww......xx");
    strcpy(vgrid[14], "xxxxxxxxxx.....wwwwwv......vwwwww.....xx");
    strcpy(vgrid[15], "xxxxxxxxxx....wwwwwvv......vvwwwww....xx");
    strcpy(vgrid[16], "xxxxxxxxx....wwwwwvv........vvwwwww....x");
    strcpy(vgrid[17], "xxxxxxxxx....wwvvvv.^^^vv^^^.vvvvww....x");
    strcpy(vgrid[18], "xxxxxxxxx...wwwv9...^=vvvv=^...9vwww...x");
    strcpy(vgrid[19], "xxxxxxxxx...wwwv....vv8vv8vv....vwww...x");
    strcpy(vgrid[20], "xxxxxxxxx^^wwwwv...vvvv||vvvv...vwwww^^x");
    strcpy(vgrid[21], "xxxxxxxxx^^wwwwv...vvvv||vvvv...vwwww^^x");
    strcpy(vgrid[22], "xxxxxxxxx^^wwwwv....vv8vv8vv....vwwww^^x");
    strcpy(vgrid[23], "xxxxxxxxx...wwwv9...^=vvvv=^...9vwww...x");
    strcpy(vgrid[24], "xxxxxxxxx...wwwvvvv.^^^vv^^^.vvvvwww...x");
    strcpy(vgrid[25], "xxxxxxxxx....wwwwwvv........vvwwwww....x");
    strcpy(vgrid[26], "xxxxxxxxxx...wwwwwwvv......vvwwwwww...xx");
    strcpy(vgrid[27], "xxxxxxxxxx....wwwwwwv......vwwwwww....xx");
    strcpy(vgrid[28], "xxxxxxxxxx.....wwwwwv9....9vwwwww.....xx");
    strcpy(vgrid[29], "xxxxxxxxxxx.....wwwwvvvvvvvvwwww.....xxx");
    strcpy(vgrid[30], "xxxxxxxxxxx.......wwwwwswwwwww.......xxx");
    strcpy(vgrid[31], "xxxxxxxxxxxx.........wwwsww.........xxxx");
    strcpy(vgrid[32], "xxxxxxxxxxxxx.......^^^^^^^^.......xxxxx");
    strcpy(vgrid[33], "xxxxxxxxxxxxxxx.....^^x==x^^.....xxxxxxx");
    strcpy(vgrid[34], "xxxxxxxxxxxxxxxxxx...xx==xx...xxxxxxxxxx");
    strcpy(vgrid[35], "xxxxxxxxxxxxxxxxxxxxxx..@.xxxxxxxxxxxxxx");

    replace_with_func( vgrid, 40, 35, '^', trapped_area );

    return MAP_NORTHWEST;
}


static char vault_9(char vgrid[81][81], FixedVector<int, 7>& mons_array)
{     // another thingy vault
    UNUSED( mons_array );

    for (unsigned char i = 0; i < 81; i++)
        strcpy(vgrid[i], "xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx");

    strcat(vgrid[36], "xxxxxxxxxxxxxxx@xxxxxxxxxxxxxxxxxxxxxxxx");
    strcat(vgrid[37], "xxxxxxxxxxxxxxx^xxxxxxxxxxxxxxxxxxxxxxxx");
    strcat(vgrid[38], "xxxxxxxxxxxxxxx^xxxxxxxxxxxxxxxxxxxxxxxx");
    strcat(vgrid[39], "xx.....^^^^^^^^^^^^^^^^^.....xxxxxxxxxxx");
    strcat(vgrid[40], "x..bb..xxxxxxxxxxxxxxxxx..bb..xxxxxxxxxx");
    strcat(vgrid[41], "x..b...xxxxxxxxxxxxxxxxx...b..xxxxxxxxxx");
    strcat(vgrid[42], "x...b..xxxxbbbbbbbbbxxxx..b...xxxxxxxxxx");
    strcat(vgrid[43], "x..bb..xxbbb.......bbbxx..bb..xxxxxxxxxx");
    strcat(vgrid[44], "x......xxb....9.9....bxx......xxxxxxxxxx");
    strcat(vgrid[45], "x..bb..xbb..~~~~~~~..bbx..bb..xxxxxxxxxx");
    strcat(vgrid[46], "x...b..xb..0~~~~~~~0..bx..b...xxxxxxxxxx");
    strcat(vgrid[47], "x..b...xb..0~~~H~~~0..bx...b..xxxxxxxxxx");
    strcat(vgrid[48], "x...b..xb..0~~~~~~~0..bx..b...xxxxxxxxxx");
    strcat(vgrid[49], "x..b...xb...~~~~~~~...bx...b..xxxxxxxxxx");
    strcat(vgrid[50], "x...b..xbb.900000009.bbx..b...xxxxxxxxxx");
    strcat(vgrid[51], "x..b...xxb...........bxx...b..xxxxxxxxxx");
    strcat(vgrid[52], "x..bb..xxbbb..9.9..bbbxx..bb..xxxxxxxxxx");
    strcat(vgrid[53], "x......xxxxbbbb.bbbbxxxx......xxxxxxxxxx");
    strcat(vgrid[54], "x..bb..xxxxxxxb=bxxxxxxx..bb..xxxxxxxxxx");
    strcat(vgrid[55], "x..b...xxxxxxxx=xxxxxxxx...b..xxxxxxxxxx");
    strcat(vgrid[56], "x...b..xxxxxxxx^xxxxxxxx..b...xxxxxxxxxx");
    strcat(vgrid[57], "x..b....xxxxxxx=xxxxxxx....b..xxxxxxxxxx");
    strcat(vgrid[58], "x...b...^^^^^^^^^^^^^^^...b...xxxxxxxxxx");
    strcat(vgrid[59], "x..b....xxxxxxxxxxxxxxx....b..xxxxxxxxxx");
    strcat(vgrid[60], "x..bb..xxxxxxxxxxxxxxxxx..bb..xxxxxxxxxx");
    strcat(vgrid[61], "xx....xxxxxxxxxxxxxxxxxxx....xxxxxxxxxxx");
    strcat(vgrid[62], "xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx");
    strcat(vgrid[63], "xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx");
    strcat(vgrid[64], "xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx");
    strcat(vgrid[65], "xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx");
    strcat(vgrid[66], "xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx");
    strcat(vgrid[67], "xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx");
    strcat(vgrid[68], "xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx");
    strcat(vgrid[69], "xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx");

    replace_with_func( vgrid, 70, 60, '~', good_treasure_area );
    replace_with_func( vgrid, 70, 60, '^', trapped_area );

    return MAP_SOUTHEAST;
}


static char vault_10(char vgrid[81][81], FixedVector<int, 7>& mons_array)
{    // impenetrable vault
    UNUSED( mons_array );

    for (unsigned char i = 0; i < 81; i++)
        strcpy(vgrid[i], "xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx");

    strcat(vgrid[36], "..............@................xxxxxxxxx");
    strcat(vgrid[37], "...............................xxxxxxxxx");
    strcat(vgrid[38], "...............................xxxxxxxxx");
    strcat(vgrid[39], "...............................xxxxxxxxx");
    strcat(vgrid[40], "...............................xxxxxxxxx");
    strcat(vgrid[41], ".....cccccccccccccccc..........xxxxxxxxx");
    strcat(vgrid[42], ".....c[^^^.........9cc.........xxxxxxxxx");
    strcat(vgrid[43], ".....c^cAAAAAAAAAAc..cc........xxxxxxxxx");
    strcat(vgrid[44], ".....c^A9.^^^^^..9cc..cc.......xxxxxxxxx");
    strcat(vgrid[45], ".....c^A.BBBBBBBc..cc..cc......xxxxxxxxx");
    strcat(vgrid[46], ".....c.A^B%%%%%%cc^^cc.9c......xxxxxxxxx");
    strcat(vgrid[47], ".....c.A^B%%~~~%%cc^^cc.c......xxxxxxxxx");
    strcat(vgrid[48], ".....c.A^cc%%~~~%%cc.9A.c......xxxxxxxxx");
    strcat(vgrid[49], ".....c.A9.cc%%~~~%%cc^A.c......xxxxxxxxx");
    strcat(vgrid[50], ".....c.cc^^cc%%~~~%%B^A.c......xxxxxxxxx");
    strcat(vgrid[51], ".....c9.cc^^cc%%%%%%B^A.c......xxxxxxxxx");
    strcat(vgrid[52], ".....cc..cc..cBBBBBBB.A^c......xxxxxxxxx");
    strcat(vgrid[53], "......cc..cc9..^^^^^.9A^c......xxxxxxxxx");
    strcat(vgrid[54], ".......cc..cAAAAAAAAAAc^c......xxxxxxxxx");
    strcat(vgrid[55], "........cc9.........^^^]c......xxxxxxxxx");
    strcat(vgrid[56], ".........cccccccccccccccc......xxxxxxxxx");
    strcat(vgrid[57], "...............................xxxxxxxxx");
    strcat(vgrid[58], "...............................xxxxxxxxx");
    strcat(vgrid[59], "...............................xxxxxxxxx");
    strcat(vgrid[60], "...............................xxxxxxxxx");
    strcat(vgrid[61], "...............................xxxxxxxxx");
    strcat(vgrid[62], "xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx");
    strcat(vgrid[63], "xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx");
    strcat(vgrid[64], "xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx");
    strcat(vgrid[65], "xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx");
    strcat(vgrid[66], "xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx");
    strcat(vgrid[67], "xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx");
    strcat(vgrid[68], "xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx");
    strcat(vgrid[69], "xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx");

    replace_one_of( vgrid, 70, 60, 'A', '=', 'c' );
    replace_one_of( vgrid, 70, 60, 'B', '=', 'c' );

    replace_with_func( vgrid, 70, 60, '%', plain_treasure_area );
    replace_with_func( vgrid, 70, 60, '~', good_treasure_area );
    replace_with_func( vgrid, 70, 60, '^', trapped_area );

    return MAP_SOUTHEAST;
}


static char orc_temple(char vgrid[81][81], FixedVector<int, 7>& mons_array)
{
    for (unsigned char i = 0; i < 81; i++)
        strcpy(vgrid[i], "xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx");

    strcpy(vgrid[36], "xxxxxxxxxxxxxxxxxxxxxxx@xxxxxxxxxxxxxxxx");
    strcpy(vgrid[37], "xxxxxxxxxxxxxxxxxxxxxxx.xxxxxxxxxxxxxxxx");
    strcpy(vgrid[38], "xxxxxxxxxxxxxxxxxxxxxx...xxxxxxxxxxxxxxx");
    strcpy(vgrid[39], "xxxxxxxxxxxxxxxxxxxxxx4.4xxxxxxxxxxxxxxx");
    strcpy(vgrid[40], "xxxxxxxxx**..........x414x..........**xx");
    strcpy(vgrid[41], "xxxxxxxxx**..........x4.4x..........**xx");
    strcpy(vgrid[42], "xxxxxxxxx............=...=....4.......xx");
    strcpy(vgrid[43], "xxxxxxxxx....4..4....x...x............xx");
    strcpy(vgrid[44], "xxxxxxxxx............x...x.......4....xx");
    strcpy(vgrid[45], "xxxxxxxxx............xx.xx............xx");
    strcpy(vgrid[46], "xxxxxxxxx...4......xxxx=xxxx......6...xx");
    strcpy(vgrid[47], "xxxxxxxxx........xxx.......xxx........xx");
    strcpy(vgrid[48], "xxxxxxxxxxx...xxxx..2.....2..xxxx...xxxx");
    strcpy(vgrid[49], "xxxxxxxxxxxx=xxxx.............xxxx=xxxxx");
    strcpy(vgrid[50], "xxxxxxxxxxx...xxx.............xxx...xxxx");
    strcpy(vgrid[51], "xxxxxxxxx......x...............x......xx");
    strcpy(vgrid[52], "xxxxxxxxx..4...x...2...I...2...x...5..xx");
    strcpy(vgrid[53], "xxxxxxxxx......x...............x......xx");
    strcpy(vgrid[54], "xxxxxxxxx...4..xx.............xx..5...xx");
    strcpy(vgrid[55], "xxxxxxxxx$......x....2...2....x......$xx");
    strcpy(vgrid[56], "xxxxxxxxx$6..5..xx.....3.....xx.5...7$xx");
    strcpy(vgrid[57], "xxxxxxxxx$$$.....xxx.......xxx.....$$$xx");
    strcpy(vgrid[58], "xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx");
    strcpy(vgrid[59], "xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx");
    strcpy(vgrid[60], "xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx");
    strcpy(vgrid[61], "xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx");
    strcpy(vgrid[62], "xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx");
    strcpy(vgrid[63], "xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx");
    strcpy(vgrid[64], "xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx");
    strcpy(vgrid[65], "xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx");
    strcpy(vgrid[66], "xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx");
    strcpy(vgrid[67], "xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx");
    strcpy(vgrid[68], "xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx");
    strcpy(vgrid[69], "xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx");

    mons_array[0] = MONS_ORC_WARLORD;
    mons_array[1] = MONS_ORC_PRIEST;
    mons_array[2] = MONS_ORC_HIGH_PRIEST;
    mons_array[3] = MONS_ORC_WARRIOR;
    mons_array[4] = MONS_ORC_WIZARD;
    mons_array[5] = MONS_ORC_KNIGHT;
    mons_array[6] = MONS_ORC_SORCERER;

    return MAP_SOUTHWEST;
}

static char my_map(char vgrid[81][81], FixedVector<int, 7>& mons_array)
{     // by Matthew Ludivico

    for (unsigned char i = 0; i < 81; i++)
        strcpy(vgrid[i], "xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx");

    strcpy(vgrid[36], "xxxxxxxxxx.@.xxxxxxxxxxxxxxxxxxxxxxxxxxx");
    strcpy(vgrid[37], "xxxxxxxxxxx...xxxxxxxxxxxxxxxxxxxxxxxxxx");
    strcpy(vgrid[38], "xxxxxxxxxxxx..........................xx");
    strcpy(vgrid[39], "xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx..xx");
    strcpy(vgrid[40], "xxxxxxxxx.^^..........................xx");
    strcpy(vgrid[41], "xxxxxxxxxx.^^xx=xxxxxxxxxxxxxxxxxxxxxxxx");
    strcpy(vgrid[42], "xxxxxxxxxxx.^...11....xxxxxxxx..xxxxxxxx");
    strcpy(vgrid[43], "xxxxxxxxxxxx..x.1..6..xxx........xx..xxx");
    strcpy(vgrid[44], "xxxxxxxxxxxxx.xxxxxxxxx...vvvvv...x...xx");
    strcpy(vgrid[45], "xxxxxxxxx6..1...x.........=1..v.......xx");
    strcpy(vgrid[46], "xxxxxxxxx..1....x.........vvvvv........x");
    strcpy(vgrid[47], "xxxxxxxxx..5...xx......................x");
    strcpy(vgrid[48], "xxxxxxxxxxxxxx^==...........vvvvvvv....x");
    strcpy(vgrid[49], "xxxxxxxxxxxxxx^xx...vv=vv...vv$%$vvvvv.x");
    strcpy(vgrid[50], "xxxxxxxxxxxxxx^x...vvv1vvv...vvv*2...v.x");
    strcpy(vgrid[51], "xxxxxxxxxxxxxx^x..vvvv7.vvvv...vv.vv=v^x");
    strcpy(vgrid[52], "xxxxxxxxx..xxx^..vvvb....bvvv...vvv^...x");
    strcpy(vgrid[53], "xxxxxxxxx%%.xx..vvvvb....bvvvv.......xxx");
    strcpy(vgrid[54], "xxxxxxxxxx.....vvbbb......bbbvv.....xxxx");
    strcpy(vgrid[55], "xxxxxxxxxxx....vvb....66....bvvxxxxxxxxx");
    strcpy(vgrid[56], "xxxxxxxxxxxxxxvvvb..llllll..bvvvxxxxxxxx");
    strcpy(vgrid[57], "xxxxxxxxxvvvvvvvbb..ll45ll..bvvvvvvvvxxx");
    strcpy(vgrid[58], "xxxxxxxxxccc***=== .l3.2.l..cccccccccxxx");
    strcpy(vgrid[59], "xxxxxxxxxccc+ccvbb....ll....c..$$$$+$*cx");
    strcpy(vgrid[60], "xxxxxxxxxcc|||cbb...3llll2..=cc%*%*c$|cx");
    strcpy(vgrid[61], "xxxxxxxxxcccccccbbbbbbbbbbbccccccccccccx");
    strcpy(vgrid[62], "xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx");
    strcpy(vgrid[63], "xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx");
    strcpy(vgrid[64], "xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx");
    strcpy(vgrid[65], "xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx");
    strcpy(vgrid[66], "xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx");
    strcat(vgrid[67], "xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx");
    strcat(vgrid[68], "xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx");
    strcat(vgrid[69], "xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx");

    mons_array[0] = MONS_HELL_HOUND;
    mons_array[1] = MONS_NECROMANCER;
    mons_array[2] = MONS_WIZARD;
    mons_array[3] = MONS_ORANGE_DEMON;
    mons_array[4] = MONS_ROTTING_DEVIL;
    mons_array[5] = MONS_HELL_KNIGHT;
    mons_array[6] = MONS_GREAT_ORB_OF_EYES;

    return MAP_SOUTHWEST;
}

static char farm_and_country(char vgrid[81][81], FixedVector<int, 7>& mons_array)
{     // by Matthew Ludivico (mll6@lehigh.edu)

    for (unsigned char i = 0; i < 81; i++)
        strcpy(vgrid[i], "xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx");

    strcpy(vgrid[0],  "xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx");
    strcpy(vgrid[1],  "xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx");
    strcpy(vgrid[2],  "xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx");
    strcpy(vgrid[3],  "xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx");
    strcpy(vgrid[4],  "xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx");
    strcpy(vgrid[5],  "xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx");
    strcpy(vgrid[6],  "xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx");
    strcpy(vgrid[7],  "xxxxxxxxxxxxxxxxx..........................................xxxxxxxx}.xxxxxxxxxxx");
    strcpy(vgrid[8],  "xxxxxxxxxxxxxxxxxxx............xxxxxx....xxx.......xx...........xxxx..]xxxxxxxxx");
    strcpy(vgrid[9],  "xxxxxxxxxxxxxxx***x...........xxx..xxx............xxxx...........xx..xxxxxxxxxxx");
    strcpy(vgrid[10], "xxxxxxxxxxxxxxx|*$=...xx.xxxxxxx....xxxxxxxxxx......xx................xxxxxxxxxx");
    strcpy(vgrid[11], "xxxxxxxxxxxxxxxxxxx....xxxxxxxx......3..xxx.................x..........xxxxxxxxx");
    strcpy(vgrid[12], "xxxxxxxxxxxxxxxxxx......x........x......xx.........w...................xxxxxxxxx");
    strcpy(vgrid[13], "xxxxxxxxxxx)......xx...xxx.....xxx......x........www3....3.............xxxxxxxxx");
    strcpy(vgrid[14], "xxxxxxxxxxxx=xxxxxxxxxxx...xxxxxxxxx..xxx.....wwwww....%%%.............xxxxxxxxx");
    strcpy(vgrid[15], "xxxxxxxxxx......xxx.......xx.xxxx.x...xxxxxxxwwwwwww..5%%%..........xx.xxxxxxxxx");
    strcpy(vgrid[16], "xxxxxxxxx.........x..xxxxxxxx.....x........3wwwwwwwww..%%%........xxx..xxxxxxxxx");
    strcpy(vgrid[17], "xxxxxxxxx....5...xx..x.xxxxx.....xxx........wwwwwwwww..%%%..........xx.xxxxxxxxx");
    strcpy(vgrid[18], "xxxxxxxxxxx.....xxx..xx..xx........xxxxxxxxxwwwwwwwww..............xxx.xxxxxxxxx");
    strcpy(vgrid[19], "xxxxxxxxxx........x..x...............xx..xxxxwwwwwwwwwwwwww............xxxxxxxxx");
    strcpy(vgrid[20], "xxxxxxxxx.............................x.....xxwwwwww3wwwwww............xxxxxxxxx");
    strcpy(vgrid[21], "xxxxxxxxxxx...x...........5.....7...............ww.......ww.....44....xxxxxxxxxx");
    strcpy(vgrid[22], "xxxxxxxxxwxx..xx.....622...2.26...6.2...22.6...62..2..226ww.....44xx...xxxxxxxxx");
    strcpy(vgrid[23], "xxxxxxxxxwwxxxx......2....2.22....2..2...2.2.......22...2ww....xxxx..xxxxxxxxxxx");
    strcpy(vgrid[24], "xxxxxxxxxwwwwxxx......2...2.2.2...2.22..2.22...22.2.2..22ww.....xxx....xxxxxxxxx");
    strcpy(vgrid[25], "xxxxxxxxxwwwwwx....4..2...2...........22...277..2..2.2.22ww...........xxxxxxxxxx");
    strcpy(vgrid[26], "xxxxxxxxxwwwwwxx....42..2....22.4..2..2...2.4..2.22..22.2ww............xxxxxxxxx");
    strcpy(vgrid[27], "xxxxxxxxxwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwww.wwwwwwwwwwwww..2.........xxxxxxxxx");
    strcpy(vgrid[28], "xxxxxxxxxwwwwwxx.....62....2.26...62.2.2..26...6...22..26..............xxxxxxxxx");
    strcpy(vgrid[29], "xxxxxxxxxwwwww.........................................................xxxxxxxxx");
    strcpy(vgrid[30], "xxxxxxxxxwwwwwxx....222.2.22..2.7.......7..............................xxxxxxxxx");
    strcpy(vgrid[31], "xxxxxxxxxwwwww...........ccccccc=ccccccc...ccc......cc+ccc...xxxxx.....xxxxxxxxx");
    strcpy(vgrid[32], "xxxxxxxxxwwwwwxx.........c$$*.c$$5$+.5.c...+5c......c%%%%c......xxx3...xxxxxxxxx");
    strcpy(vgrid[33], "xxxxxxxxxwwwwwx....2.....c$.c+cccccc.%.c...ccc......c%%%%c....xxxxx....xxxxxxxxx");
    strcpy(vgrid[34], "xxxxxxxxxwwwwwx..........c..c..........c............cccccc......xxx....xxxxxxxxx");
    strcpy(vgrid[35], "xxxxxxxxxwwxxxxxxx.......ccccc=ccccccccc.........................xx....xxxxxxxxx");
    strcpy(vgrid[36], "xxxxxxxxxwxx.....xxxx........c...c.................2...................xxxxxxxxx");
    strcpy(vgrid[37], "xxxxxxxxxxx.........xxxx...........2....xxxx...........................xxxxxxxxx");
    strcpy(vgrid[38], "xxxxxxxxx..............xxxx..........xxxx..x...........................xxxxxxxxx");
    strcpy(vgrid[39], "xxxxxxxxx.................xxxxc++cxxxx.....xx............xx...x........xxxxxxxxx");
    strcpy(vgrid[40], "xxxxxxxxx.....................c..c..........xxxxx..........xxxxx.......xxxxxxxxx");
    strcpy(vgrid[41], "xxxxxxxxx.......cccc..........c..c...cccc......xxx...........x.........xxxxxxxxx");
    strcpy(vgrid[42], "xxxxxxxxx.......c..c..........c++c...c..c........xxx.........x.........xxxxxxxxx");
    strcpy(vgrid[43], "xxxxxxxxx.......c..c..........c..c...c..c..........xxx.................xxxxxxxxx");
    strcpy(vgrid[44], "xxxxxxxxx....cccc++cccccccccccc++ccccc..ccccccc......xxx...............xxxxxxxxx");
    strcpy(vgrid[45], "xxxxxxxxx....c..........1.....................c........xxx.............xxxxxxxxx");
    strcpy(vgrid[46], "xxxxxxxxx.cccc.....w....w....%1.....w.....%...c..........xxx...........xxxxxxxxx");
    strcpy(vgrid[47], "xxxxxxxxx.c1.+....www..www..%%%....www...%%%1.c...........xxxxxxxxx....xxxxxxxxx");
    strcpy(vgrid[48], "xxxxxxxxx.cccc.....w....w....%......w.....%...c..................xxx...xxxxxxxxx");
    strcpy(vgrid[49], "xxxxxxxxx....c.......5........................c....................xxxxxxxxxxxxx");
    strcpy(vgrid[50], "xxxxxxxxx....ccc....%%%%%....cccccccccccccccccc........................xxxxxxxxx");
    strcpy(vgrid[51], "xxxxxxxxx......cc...........cc.........................................xxxxxxxxx");
    strcpy(vgrid[52], "xxxxxxxxx.......cccccc+cccccc..........................................xxxxxxxxx");
    strcpy(vgrid[53], "xxxxxxxxx........cc.......cc...........................................xxxxxxxxx");
    strcpy(vgrid[54], "xxxxxxxxx.........cc.....cc.....................ccccccccccccccccccccccccxxxxxxxx");
    strcpy(vgrid[55], "xxxxxxxxx..........ccc.ccc......................c......vvv.............cxxxxxxxx");
    strcpy(vgrid[56], "xxxxxxxxx..........ccc.c........................c......v5+...vvvvv.....cxxxxxxxx");
    strcpy(vgrid[57], "xxxxxxxxx..........ccc.c........................c......vvv...v.5.v.....cxxxxxxxx");
    strcpy(vgrid[58], "xxxxxxxxxccccccccccccc=ccc......................c............v..5v.....cxxxxxxxx");
    strcpy(vgrid[59], "xxxxxxxxx..........c.....cccccccccccccccccccccccccccc..........vv+vv...cxxxxxxxx");
    strcpy(vgrid[60], "xxxxxxxxx..........c.....=......................+................5111..cxxxxxxxx");
    strcpy(vgrid[61], "xxxxxxxxx..........c.{([.c......................+................5.....cxxxxxxxx");
    strcpy(vgrid[62], "xxxxxxxxxxxxxxxxxxxcccccccccccccccccccccccccccccccccccccccccccccccccccccxxxxxxxx");
    strcpy(vgrid[63], "xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx");
    strcpy(vgrid[64], "xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx");
    strcpy(vgrid[65], "xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx");
    strcpy(vgrid[66], "xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx");
    strcpy(vgrid[67], "xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx");
    strcpy(vgrid[68], "xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx");
    strcpy(vgrid[69], "xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx");

    mons_array[0] = MONS_DEATH_YAK;
    mons_array[1] = MONS_PLANT;
    mons_array[2] = MONS_GRIFFON;
    mons_array[3] = MONS_KILLER_BEE;
    mons_array[4] = MONS_OGRE;
    mons_array[5] = MONS_OKLOB_PLANT;
    mons_array[6] = MONS_WANDERING_MUSHROOM;

    return MAP_ENCOMPASS;
}


static char fort_yaktaur(char vgrid[81][81], FixedVector<int, 7>& mons_array)
{     // by Matthew Ludivico (mll6@lehigh.edu)

    for (unsigned char i = 0; i < 81; i++)
        strcpy(vgrid[i], "xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx");

    strcat(vgrid[36], ".........@....wwwwwwwwwwwwwwwwwxxxxxxxxx");
    strcat(vgrid[37], ".ccccc.......ww....wwww....wwwwxxxxxxxxx");
    strcat(vgrid[38], ".c$c%c......ww.ccccccccc.......xxxxxxxxx");
    strcat(vgrid[39], ".c+c+c......ww.c.%$....ccccccccxxxxxxxxx");
    strcat(vgrid[40], ".c...+......ww.c*.115..c$$+|*|cxxxxxxxxx");
    strcat(vgrid[41], ".c1..c.....ww..c...55+ccc+cxx=cxxxxxxxxx");
    strcat(vgrid[42], ".ccccc.....ww..ccccccc....c|=*cxxxxxxxxx");
    strcat(vgrid[43], "............ww.......c5...cxx=cxxxxxxxxx");
    strcat(vgrid[44], "....6.ccccc.ww.w...2.+51..c|1.cxxxxxxxxx");      // last 1 here was 7
    strcat(vgrid[45], "....63+...c..wwww..21+51..c2.2cxxxxxxxxx");
    strcat(vgrid[46], "....6.ccccc..wwwwww..c5...cc+ccxxxxxxxxx");
    strcat(vgrid[47], "............wwwwwww..c........cxxxxxxxxx");
    strcat(vgrid[48], "............wwwwwww..ccccccccccxxxxxxxxx");
    strcat(vgrid[49], "...........ww1w..www...........xxxxxxxxx");
    strcat(vgrid[50], ".......566.www.....www.........xxxxxxxxx");
    strcat(vgrid[51], ".........1ww....ccccc..........xxxxxxxxx");
    strcat(vgrid[52], ".....566.w......+...c..........xxxxxxxxx");
    strcat(vgrid[53], ".........www....ccccc..........xxxxxxxxx");
    strcat(vgrid[54], "...........ww............wwwwwwxxxxxxxxx");
    strcat(vgrid[55], ".......3....wwwww......www.....xxxxxxxxx");
    strcat(vgrid[56], "......666.......ww...www.......xxxxxxxxx");
    strcat(vgrid[57], ".....cc+cc.......wwwww.........xxxxxxxxx");
    strcat(vgrid[58], ".....c...c.....................xxxxxxxxx");
    strcat(vgrid[59], ".....ccccc.....................xxxxxxxxx");
    strcat(vgrid[60], "...............................xxxxxxxxx");
    strcat(vgrid[61], "...............................xxxxxxxxx");
    strcat(vgrid[62], "xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx");
    strcat(vgrid[63], "xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx");
    strcat(vgrid[64], "xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx");
    strcat(vgrid[65], "xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx");
    strcat(vgrid[66], "xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx");
    strcat(vgrid[67], "xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx");
    strcat(vgrid[68], "xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx");
    strcat(vgrid[69], "xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx");

    mons_array[0] = MONS_YAKTAUR;
    mons_array[1] = MONS_DEATH_YAK;
    mons_array[2] = MONS_MINOTAUR;
    mons_array[3] = RANDOM_MONSTER;
    mons_array[4] = MONS_YAK;
    mons_array[5] = MONS_GNOLL;
    mons_array[6] = RANDOM_MONSTER;

    return MAP_SOUTHEAST;
}


static char box_level(char vgrid[81][81], FixedVector<int, 7>& mons_array)
{     // by John Savard
    UNUSED( mons_array );

    strcpy(vgrid[0],  "xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx");
    strcpy(vgrid[1],  "xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx");
    strcpy(vgrid[2],  "xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx");
    strcpy(vgrid[3],  "xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx");
    strcpy(vgrid[4],  "xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx");
    strcpy(vgrid[5],  "xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx");
    strcpy(vgrid[6],  "xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx");
    strcpy(vgrid[7],  "xxxxxxxxx.................xx.............x...................^.........xxxxxxxxx");
    strcpy(vgrid[8],  "xxxxxxxxx.................xx...xxxxxx....x.xxxxxxx.xxxxxxxxxxxxxxxxxxx.xxxxxxxxx");
    strcpy(vgrid[9],  "xxxxxxxxx.................xx...xx.0......x.x........x......x.........x.xxxxxxxxx");
    strcpy(vgrid[10], "xxxxxxxxx..$..............xx...xx........x.x........x.....%x.x..*..xxx.xxxxxxxxx");
    strcpy(vgrid[11], "xxxxxxxxx......................xx........x.x........x.xxxxxx.x.....x...xxxxxxxxx");
    strcpy(vgrid[12], "xxxxxxxxx......................xx....%...x.x........x.x......xxxxxxx.x.xxxxxxxxx");
    strcpy(vgrid[13], "xxxxxxxxx.................xx...xx........x.x........x.x.xxxxxx.......x.xxxxxxxxx");
    strcpy(vgrid[14], "xxxxxxxxx.................xx...xx........x.x..{.....x.x..............x.xxxxxxxxx");
    strcpy(vgrid[15], "xxxxxxxxx.............0...xx...xxxxxxxxxxx.xxxxxxxxxx.xxxxxxxxxxxxxxxx.xxxxxxxxx");
    strcpy(vgrid[16], "xxxxxxxxx.................xx...........................................xxxxxxxxx");
    strcpy(vgrid[17], "xxxxxxxxxxxxxxxxxxxxxxxxxxxx...xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx");
    strcpy(vgrid[18], "xxxxxxxxxxxxxxxxxxxxxxxxxxxx...xxx}x.........................>=........xxxxxxxxx");
    strcpy(vgrid[19], "xxxxxxxxx..................x...xxx.x.xxx+xxxxxxxxxxxxxxxx+xxxxx........xxxxxxxxx");
    strcpy(vgrid[20], "xxxxxxxxx..xxxxxxxxxxxxxx..x...xxx.x.x0...x..0..............0.x........xxxxxxxxx");
    strcpy(vgrid[21], "xxxxxxxxx..x............x..x...xxx.x.x....x...................x........xxxxxxxxx");
    strcpy(vgrid[22], "xxxxxxxxx....xxxxxxxxx..x..x...xxx.x.x....x...................x......8*xxxxxxxxx");
    strcpy(vgrid[23], "xxxxxxxxx..x.x....0..x..x..x...xxx...x...%x...................x......*|xxxxxxxxx");
    strcpy(vgrid[24], "xxxxxxxxx..x.x..........x..x...xxxxxxxxxxxxxxxxxxxxxx.xxxxxxxxxxxxxxxxxxxxxxxxxx");
    strcpy(vgrid[25], "xxxxxxxxx..x.x*......x..x..x..........x...........0...x...%............xxxxxxxxx");
    strcpy(vgrid[26], "xxxxxxxxx..x.xxxxxxxxx..x..=..........x.xxxxxxxxxxxxx.x................xxxxxxxxx");
    strcpy(vgrid[27], "xxxxxxxxx..x......0.....xxxxxxx.......x.x...x...x...x.x................xxxxxxxxx");
    strcpy(vgrid[28], "xxxxxxxxx..xxxxxxxxxxxxxxxxxxxx..0....x...x.x.x.x.x.x.x......0.........xxxxxxxxx");
    strcpy(vgrid[29], "xxxxxxxxx..........^.........xx.......x.x.x.x.x.x.x...+................xxxxxxxxx");
    strcpy(vgrid[30], "xxxxxxxxxcccccccccccccccccc..xx.......x.x$x...x...xxxxx................xxxxxxxxx");
    strcpy(vgrid[31], "xxxxxxxxxc...........9....c..xx.......x.x.xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx");
    strcpy(vgrid[32], "xxxxxxxxxc......c............xx.......x.x.x...x..0.....................xxxxxxxxx");
    strcpy(vgrid[33], "xxxxxxxxxc.....|c............xx.......x.x.x.x.x........................xxxxxxxxx");
    strcpy(vgrid[34], "xxxxxxxxxc...........9....c..xx.......x.x...x.x........................xxxxxxxxx");
    strcpy(vgrid[35], "xxxxxxxxxcccccccccccccccccc..xx.......x.xxxxx.x........................xxxxxxxxx");
    strcpy(vgrid[36], "xxxxxxxxx....................xx.......x.x.....=....................*...xxxxxxxxx");
    strcpy(vgrid[37], "xxxxxxxxx....................xx.......x.x.xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx");
    strcpy(vgrid[38], "xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx.......x.x.x...........................(xxxxxxxxx");
    strcpy(vgrid[39], "xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx.xxxxxxx.x$x..xxxx.xxxxxxxxxxxxxxxxxxxx.xxxxxxxxx");
    strcpy(vgrid[40], "xxxxxxxxx...............................x.x..x.......................x.xxxxxxxxx");
    strcpy(vgrid[41], "xxxxxxxxx..xxxxxxxxxxxxxxxxxxxxxxxxxxxxxx.x..x.xxxxxxxxxxxxx.........x.xxxxxxxxx");
    strcpy(vgrid[42], "xxxxxxxxx.............)xxx................x..x.xxxxxxxxxxxxx.........x.xxxxxxxxx");
    strcpy(vgrid[43], "xxxxxxxxx..............xxx.xxxxxxxxxxxxxxxx..x.xxxxxxxxxxxxx.........x.xxxxxxxxx");
    strcpy(vgrid[44], "xxxxxxxxx..............xxx...................x.x...........xxxxx+xxxxx.xxxxxxxxx");
    strcpy(vgrid[45], "xxxxxxxxx..............xxxxxxxxxxxxxxxxxxxxxxx.x..$........x.........x.xxxxxxxxx");
    strcpy(vgrid[46], "xxxxxxxxx......9.......xxxxxxxxxxxxxxxxxxxxxxx.x...........x........%x.xxxxxxxxx");
    strcpy(vgrid[47], "xxxxxxxxx..............xxxxxxxxxxxxxxxxxxxxxxx.x.0.........x0........x.xxxxxxxxx");
    strcpy(vgrid[48], "xxxxxxxxx..............xxxxxxxxxxxxxxxxxxxxxxx.x.......$...x.........x.xxxxxxxxx");
    strcpy(vgrid[49], "xxxxxxxxx..............xxxxxxxxxxxxxxxxxxxxxxx.x...........xxxxxxxxxxx.xxxxxxxxx");
    strcpy(vgrid[50], "xxxxxxxxx..............xxxxxxxxxxxxxxxxxxxxxxx.xxxxxxxxxxx.x...........xxxxxxxxx");
    strcpy(vgrid[51], "xxxxxxxxx..............xxxxxxxxxxxxxxxxxxxxxxx.............x...........xxxxxxxxx");
    strcpy(vgrid[52], "xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx+xxxxxxxxxxxxxxxxxx");
    strcpy(vgrid[53], "xxxxxxxxx..............................................................xxxxxxxxx");
    strcpy(vgrid[54], "xxxxxxxxx.xxxxxxxxxxxxxxxxxxx.xxxxxxxxxxxxxxxxxx.xxxxxxx=xxxxxx.xxxxxx.xxxxxxxxx");
    strcpy(vgrid[55], "xxxxxxxxx.....xx.................xxxxxxxxxxx.......x........x.....x....xxxxxxxxx");
    strcpy(vgrid[56], "xxxxxxxxx....0xx.................xxxxxxxxxxx.%.....x.0......x...0.x....xxxxxxxxx");
    strcpy(vgrid[57], "xxxxxxxxx.....xx.9...............xxxxxxxxxxx.......x........x.%...x..$.xxxxxxxxx");
    strcpy(vgrid[58], "xxxxxxxxx.....xx.................xxxxxxxxxxx.......x........x.....x....xxxxxxxxx");
    strcpy(vgrid[59], "xxxxxxxxx.....xx.................xxxxxxxxxxx.......x........x.....x..0.xxxxxxxxx");
    strcpy(vgrid[60], "xxxxxxxxx....0xx.................xxxxxxxxxxx.......x$.......x.....x....xxxxxxxxx");
    strcpy(vgrid[61], "xxxxxxxxx]....xx................*xxxxxxxxxxx......[x........x.....x$...xxxxxxxxx");
    strcpy(vgrid[62], "xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx");
    strcpy(vgrid[63], "xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx");
    strcpy(vgrid[64], "xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx");
    strcpy(vgrid[65], "xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx");
    strcpy(vgrid[66], "xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx");
    strcpy(vgrid[67], "xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx");
    strcpy(vgrid[68], "xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx");
    strcpy(vgrid[69], "xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx");

    return MAP_ENCOMPASS;
}

// Note: that the Arches are converted into the correct gateway by looking at
// the grid two to the right (floor, stone, water, lava).
static char vestibule_map(char vgrid[81][81], FixedVector<int, 7>& mons_array)
{
    strcpy(vgrid[0],  "cccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccc");
    strcpy(vgrid[1],  "cccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccc");
    strcpy(vgrid[2],  "cccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccc");
    strcpy(vgrid[3],  "cccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccc");
    strcpy(vgrid[4],  "cccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccc");
    strcpy(vgrid[5],  "cccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccc");
    strcpy(vgrid[6],  "ccccccccccccccccccccccccccccccccccccvvvvvvvccccccccccccccccccccccccccccccccccccc");
    strcpy(vgrid[7],  "cccccccccccccccccccccccccccccccccc..v.....v..ccccccccccccccccccccccccccccccccccc");
    strcpy(vgrid[8],  "ccccccccccccccccccccccccccccccc.....v.....v.....cccccccccccccccccccccccccccccccc");
    strcpy(vgrid[9],  "cccccccccccccccccccccccccccc........v.....v........ccccccccccccccccccccccccccccc");
    strcpy(vgrid[10], "cccccccccccccccccccccccccc..........v..A..v..........ccccccccccccccccccccccccccc");
    strcpy(vgrid[11], "cccccccccccccccccccccccc............v.....v............ccccccccccccccccccccccccc");
    strcpy(vgrid[12], "ccccccccccccccccccccccc.............v.....v.............cccccccccccccccccccccccc");
    strcpy(vgrid[13], "cccccccccccccccccccccc..............vvv=vvv..............ccccccccccccccccccccccc");
    strcpy(vgrid[14], "ccccccccccccccccccccc.....................................cccccccccccccccccccccc");
    strcpy(vgrid[15], "cccccccccccccccccccc.......................................ccccccccccccccccccccc");
    strcpy(vgrid[16], "ccccccccccccccccccc.........................................cccccccccccccccccccc");
    strcpy(vgrid[17], "cccccccccccccccccc...........................................ccccccccccccccccccc");
    strcpy(vgrid[18], "ccccccccccccccccc.............................................cccccccccccccccccc");
    strcpy(vgrid[19], "cccccccccccccccc...............................................ccccccccccccccccc");
    strcpy(vgrid[20], "ccccccccccccccc.................................................cccccccccccccccc");
    strcpy(vgrid[21], "cccccccccccccc...................................................ccccccccccccccc");
    strcpy(vgrid[22], "ccccccccccccc.....................................................cccccccccccccc");
    strcpy(vgrid[23], "ccccccccccccc.....................................................cccccccccccccc");
    strcpy(vgrid[24], "cccccccccccc.......................................................ccccccccccccc");
    strcpy(vgrid[25], "cccccccccccc.......................................................ccccccccccccc");
    strcpy(vgrid[26], "ccccccccccc.........................................................cccccccccccc");
    strcpy(vgrid[27], "ccccccccccc............................{............................cccccccccccc");
    strcpy(vgrid[28], "ccccccccccc.........................................................cccccccccccc");
    strcpy(vgrid[29], "cccccccccc...l.l.....................................................ccccccccccc");
    strcpy(vgrid[30], "cccccccccc..l.l.l.l..................................................ccccccccccc");
    strcpy(vgrid[31], "cccccccccc.l.l.l.l.l.................................................ccccccccccc");
    strcpy(vgrid[32], "ccccccccc.l.l.l.l.l...................................................cccccccccc");
    strcpy(vgrid[33], "cccccccccl.l.l.l.l.l..................................................cccccccccc");
    strcpy(vgrid[34], "ccccccccc.l.l.l.A.l.l.................}1].............................=Acccccccc");
    strcpy(vgrid[35], "cccccccccl.l.l.l.l.l.l.................)..............................cccccccccc");
    strcpy(vgrid[36], "ccccccccc.l.l.l.l.l.l.................................................cccccccccc");
    strcpy(vgrid[37], "cccccccccc.l.l.l.l.l.l...............................................ccccccccccc");
    strcpy(vgrid[38], "cccccccccc..l.l.l.l..................................................ccccccccccc");
    strcpy(vgrid[39], "cccccccccc.....l.l...................................................ccccccccccc");
    strcpy(vgrid[40], "ccccccccccc......................[...........(......................cccccccccccc");
    strcpy(vgrid[41], "ccccccccccc.........................................................cccccccccccc");
    strcpy(vgrid[42], "ccccccccccc.........................................................cccccccccccc");
    strcpy(vgrid[43], "cccccccccccc.......................................................ccccccccccccc");
    strcpy(vgrid[44], "cccccccccccc.......................................................ccccccccccccc");
    strcpy(vgrid[45], "ccccccccccccc.....................................................cccccccccccccc");
    strcpy(vgrid[46], "ccccccccccccc.....................................................cccccccccccccc");
    strcpy(vgrid[47], "cccccccccccccc...................................................ccccccccccccccc");
    strcpy(vgrid[48], "ccccccccccccccc....................wwwww........................cccccccccccccccc");
    strcpy(vgrid[49], "cccccccccccccccc..................wwwwwwww.....................ccccccccccccccccc");
    strcpy(vgrid[50], "ccccccccccccccccc..............wwwwwwwwwwwww..................cccccccccccccccccc");
    strcpy(vgrid[51], "cccccccccccccccccc...........w..wwww..wwwww..w...............ccccccccccccccccccc");
    strcpy(vgrid[52], "ccccccccccccccccccc..........w...ww.....ww..wwwww...........cccccccccccccccccccc");
    strcpy(vgrid[53], "cccccccccccccccccccc.........ww......ww....wwwwwwwww.......ccccccccccccccccccccc");
    strcpy(vgrid[54], "ccccccccccccccccccccc.........ww....wwww...wwwwwwwwww.....cccccccccccccccccccccc");
    strcpy(vgrid[55], "cccccccccccccccccccccc.........ww....ww....wwwwwwwwwww...ccccccccccccccccccccccc");
    strcpy(vgrid[56], "ccccccccccccccccccccccc........wwww.......wwwwwwwwwwwwwwcccccccccccccccccccccccc");
    strcpy(vgrid[57], "cccccccccccccccccccccccc......wwwwwww....wwwwwwwwwwwwwwccccccccccccccccccccccccc");
    strcpy(vgrid[58], "cccccccccccccccccccccccccc...wwwwwwwwwwAwwwwwwwwwwwwwccccccccccccccccccccccccccc");
    strcpy(vgrid[59], "ccccccccccccccccccccccccccccwwwwwwwwwwwwwwwwwwwwwwwccccccccccccccccccccccccccccc");
    strcpy(vgrid[60], "cccccccccccccccccccccccccccccccwwwwwwwwwwwwwwwwwcccccccccccccccccccccccccccccccc");
    strcpy(vgrid[61], "ccccccccccccccccccccccccccccccccccwwwwwwwwwwwccccccccccccccccccccccccccccccccccc");
    strcpy(vgrid[62], "cccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccc");
    strcpy(vgrid[63], "cccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccc");
    strcpy(vgrid[64], "cccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccc");
    strcpy(vgrid[65], "cccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccc");
    strcpy(vgrid[66], "cccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccc");
    strcpy(vgrid[67], "cccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccc");
    strcpy(vgrid[68], "cccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccc");
    strcpy(vgrid[69], "cccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccc");

    mons_array[0] = MONS_GERYON;
    mons_array[1] = RANDOM_MONSTER;
    mons_array[2] = RANDOM_MONSTER;
    mons_array[3] = RANDOM_MONSTER;
    mons_array[4] = RANDOM_MONSTER;
    mons_array[5] = RANDOM_MONSTER;
    mons_array[6] = RANDOM_MONSTER;

    return MAP_ENCOMPASS;
}


static char castle_dis(char vgrid[81][81], FixedVector<int, 7>& mons_array)
{     // Dispater's castle - rest of level filled up with plan_4 (irregular city)

    strcpy(vgrid[0],  "xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx");
    strcpy(vgrid[1],  "xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx");
    strcpy(vgrid[2],  "xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx");
    strcpy(vgrid[3],  "xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx");
    strcpy(vgrid[4],  "xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx");
    strcpy(vgrid[5],  "xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx");
    strcpy(vgrid[6],  "xxxxxxxx................................................................xxxxxxxx");
    strcpy(vgrid[7],  "xxxxxxxx................................................................xxxxxxxx");
    strcpy(vgrid[8],  "xxxxxxxx...vvvvvvvvv........................................vvvvvvvvv...xxxxxxxx");
    strcpy(vgrid[9],  "xxxxxxxx...v3.....|v........................................v|.....2v...xxxxxxxx");
    strcpy(vgrid[10], "xxxxxxxx...v.vv=vvvv.v.v.v.v.v.v.v.v.v..v.v.v.v.v.v.v.v.v.v.vvvv=vv.v...xxxxxxxx");
    strcpy(vgrid[11], "xxxxxxxx...v.v.....vvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvv.....v.v...xxxxxxxx");
    strcpy(vgrid[12], "xxxxxxxx...v|v.....=$$v$$=||v$$vvvvvvvvvvvvvvvvv$$$$v4.4.v$$v.....v|v...xxxxxxxx");
    strcpy(vgrid[13], "xxxxxxxx...vvvv.vvvv$$=$$v||v$$=....2.....2....=$$$$v.4.4=$$v=vv=vvvv...xxxxxxxx");
    strcpy(vgrid[14], "xxxxxxxx.....vv=vvvvvvvvvvvvvvvv.v..v..v..v..v.v$$$$v4.4.v$$=||v.vv5....xxxxxxxx");
    strcpy(vgrid[15], "xxxxxxxx....vvv................v...............vvvvvvvvvvvvvvvvv.vvv....xxxxxxxx");
    strcpy(vgrid[16], "xxxxxxxx....5vv................=...............=.................vv.....xxxxxxxx");
    strcpy(vgrid[17], "xxxxxxxx....vvv+vvvvvvvvvvvvvvvv4v..v..v..v..v4vvvvvvvvvvvvvvvvv.vvv....xxxxxxxx");
    strcpy(vgrid[18], "xxxxxxxx.....vv..v.=$$$$$v.....v...............vvvvvvvvvvvvvvvvv.vv5....xxxxxxxx");
    strcpy(vgrid[19], "xxxxxxxx....vvv..v.v$$$$$v.....v...............vv|$|$|vv|$|$|$vv.vvv....xxxxxxxx");
    strcpy(vgrid[20], "xxxxxxxx....5vv..v.vvvvvvv.....vvvvv.......vvvvvv$|$|$++$|$|$|vv.vv.....xxxxxxxx");
    strcpy(vgrid[21], "xxxxxxxx....vvv..v.............=3v.vvvv+vvvvvvvvvvvvvvvvvvvvv+vv.vvv....xxxxxxxx");
    strcpy(vgrid[22], "xxxxxxxx.....vvv+v..........vvvvv.4vvv...vvvvvvvvvvvvvvvvvvvv+vv.vv5....xxxxxxxx");
    strcpy(vgrid[23], "xxxxxxxx....vvv..v.v..v..v....2vvv=vv5...5vvvvvvv.4.4.vv.4.4.4vv.vvv....xxxxxxxx");
    strcpy(vgrid[24], "xxxxxxxx....5vv.................vv|vvv...vvvvv.++4.4.4++4.4.4.vv.vv.....xxxxxxxx");
    strcpy(vgrid[25], "xxxxxxxx....vvv.................1v|vv5...5vvvv.vvvvvvvvvvvvvvvvv.vvv....xxxxxxxx");
    strcpy(vgrid[26], "xxxxxxxx.....vv.................vv|vvv...vvvvv.vvvvvvvvvvvvvvvvv.vv5....xxxxxxxx");
    strcpy(vgrid[27], "xxxxxxxx....vvv.v..v..v..v....3vvv=vv5...5vvvv...................vvv....xxxxxxxx");
    strcpy(vgrid[28], "xxxxxxxx....5vv.............vvvvv.4vvv...vvvvvvvvvvvvvvvvvvvvvvv=vv.....xxxxxxxx");
    strcpy(vgrid[29], "xxxxxxxx...vvvv=vvvv...........=2v.vv5...5vvvvvvvvvvvvvvvvvvvvvv.vvvv...xxxxxxxx");
    strcpy(vgrid[30], "xxxxxxxx...v|v.....vvvvvvvvvvvvvvvvvvv...vvvvvvvvvvvvvvvvvvvv.....v|v...xxxxxxxx");
    strcpy(vgrid[31], "xxxxxxxx...v.v.....vvvvvvvvvvvvvvvvvvvv=vvvvvvvvvvvvvvvvvvvvv.....v.v...xxxxxxxx");
    strcpy(vgrid[32], "xxxxxxxx...v.vv=vvvv5.............5.........5..............5vvvv=vv.v...xxxxxxxx");
    strcpy(vgrid[33], "xxxxxxxx...v2.....|v........................................v|.....3v...xxxxxxxx");
    strcpy(vgrid[34], "xxxxxxxx...vvvvvvvvv........................................vvvvvvvvv...xxxxxxxx");
    strcpy(vgrid[35], "xxxxxxxx.............................{.[.(..............................xxxxxxxx");

    mons_array[0] = MONS_DISPATER;
    mons_array[1] = MONS_FIEND;
    mons_array[2] = MONS_ICE_FIEND;
    mons_array[3] = MONS_IRON_DEVIL;
    mons_array[4] = MONS_METAL_GARGOYLE;
    mons_array[5] = RANDOM_MONSTER;
    mons_array[6] = RANDOM_MONSTER;

    return MAP_NORTH;
}


static char asmodeus(char vgrid[81][81], FixedVector<int, 7>& mons_array)
{

    strcpy(vgrid[0],  "xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx");
    strcpy(vgrid[1],  "xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx");
    strcpy(vgrid[2],  "xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx");
    strcpy(vgrid[3],  "xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx");
    strcpy(vgrid[4],  "xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx");
    strcpy(vgrid[5],  "xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx");
    strcpy(vgrid[6],  "xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx");
    strcpy(vgrid[7],  "xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx.xxxxxxxxxx....xxxxxxxxxxxxxxx.xxxxxxxxxxxx");
    strcpy(vgrid[8],  "xxxxxxxxxxxxxxxxxxxxxxxxx............................xxxxxxxxxxxxxx..xxxxxxxxxxx");
    strcpy(vgrid[9],  "xxxxxxxxxxxxxxxxxxxxxxxxxx..............................xxxxxxxxxx....xxxxxxxxxx");
    strcpy(vgrid[10], "xxxxxxxxxxxxxxxxxxxxx...xxx................................xxxxxx....xxxxxxxxxxx");
    strcpy(vgrid[11], "xxxxxxxxxxxx.x.xxxxx.........................................xxx....xxxxxxxxxxxx");
    strcpy(vgrid[12], "xxxxxxxxxxxx....xx.....................4......................xx...xxxxxxxxxxxxx");
    strcpy(vgrid[13], "xxxxxxxxxxx......x......................llllllllllllll.........x..xxxxxxxxxxxxxx");
    strcpy(vgrid[14], "xxxxxxxxxxx..xx..................lllllllllllllllllllllllll........xxxxxxxxxxxxxx");
    strcpy(vgrid[15], "xxxxxxxxxx...xxx....0..........llllllllllllllllllllllllll........xx...xxxxxxxxxx");
    strcpy(vgrid[16], "xxxxxxxxx....xxx.............llllllllllllllllllllllllllll..............xxxxxxxxx");
    strcpy(vgrid[17], "xxxxxxxxxx....xx...........lllllllllllllllllllllllllllll...............xxxxxxxxx");
    strcpy(vgrid[18], "xxxxxxxxxxxx..............llllllllllllllllllllllllllllll...2..xx...0...xxxxxxxxx");
    strcpy(vgrid[19], "xxxxxxxxxxxxx...........lllllllllllllllllll.......llllll......xx......xxxxxxxxxx");
    strcpy(vgrid[20], "xxxxxxxxxxxxxx.......llllllllllllllllll............llllll.............xxxxxxxxxx");
    strcpy(vgrid[21], "xxxxxxxxxxxxxxx......lllllllll..........4.........4.lllllll..........xxxxxxxxxxx");
    strcpy(vgrid[22], "xxxxxxxxxx...xx...ll3lllll......4...................llllllll......x.xxxxxxxxxxxx");
    strcpy(vgrid[23], "xxxxxxxxx.......lllll.l................................llll.......xxxxxxxxxxxxxx");
    strcpy(vgrid[24], "xxxxxxxxxx..4..llllll...cccccccccc=c=c=c=c=c=c=c=ccc....lll......xxxxxxxxxxxxxxx");
    strcpy(vgrid[25], "xxxxxxxxxxx..lllllll..4.c.....c....................c....llll.....xxxxxxxxxxxxxxx");
    strcpy(vgrid[26], "xxxxxxxxxx...llllll.....c.V.V.=....0.....3.....0...c.....llll....x..xxxxxxxxxxxx");
    strcpy(vgrid[27], "xxxxxxxxx...llllll...l..c.....c....................c....lllll........xxxxxxxxxxx");
    strcpy(vgrid[28], "xxxxxxxxxx...lllll..ll..c..5..cccccccccccccccccccccc.4..llllll........xxxxxxxxxx");
    strcpy(vgrid[29], "xxxxxxxxx...lllll..llll.c.....c...............c....c....lllllll.......xxxxxxxxxx");
    strcpy(vgrid[30], "xxxxxxxxx...lllll..llll.c.V.V.c.......0.......c....c....lllllll.......xxxxxxxxxx");
    strcpy(vgrid[31], "xxxxxxxxxx...lllll..lll.c.....=...............=....c...lllllll........xxxxxxxxxx");
    strcpy(vgrid[32], "xxxxxxxxxxx..lllll...ll.cccccccccc....0.......c....c...llllllll........xxxxxxxxx");
    strcpy(vgrid[33], "xxxxxxxxxx...lllll..4...c|$$||$$|c............c.0..c...llllllll........xxxxxxxxx");
    strcpy(vgrid[34], "xxxxxxxxx...lllll.......c$$$$$$$$cccccccccccccc....c...lllllll.........xxxxxxxxx");
    strcpy(vgrid[35], "xxxxxxxxx...lllll.......c$$|2|$$|c..0.........=....c...lllllll........xxxxxxxxxx");
    strcpy(vgrid[36], "xxxxxxxxxx.lllllll......c|$$$$$$$c........9...c....c....llllllll.....xxxxxxxxxxx");
    strcpy(vgrid[37], "xxxxxxxxxx.lllllll......c$|$|$$|$c=ccccccccccccccccc....lllllll......xxxxxxxxxxx");
    strcpy(vgrid[38], "xxxxxxxxxx..llllll......cccccccc=c.....9.......c.........llllll......x.xxxxxxxxx");
    strcpy(vgrid[39], "xxxxxxxxxx..lllllll.....c$$$$$$c3c.....8...3...c.....4...llllll........xxxxxxxxx");
    strcpy(vgrid[40], "xxxxxxxxxx..llllllll....c$$$$$$c.c.....9.......c..ll....llllll.........xxxxxxxxx");
    strcpy(vgrid[41], "xxxxxxxxxx...llllll..4..c$$2$$$=.ccccccccccccc=c.lll...lllllll...0....xxxxxxxxxx");
    strcpy(vgrid[42], "xxxxxxxxxxx..llllll.....c$$$$$$c..+............c.ll...lllllll..........xxxxxxxxx");
    strcpy(vgrid[43], "xxxxxxxxxxx..llllllll...ccccccccc=cccccccccccccc.....lllllll...........xxxxxxxxx");
    strcpy(vgrid[44], "xxxxxxxxxxxx..llllllll.........cc..........cc........lllllll.......x..xxxxxxxxxx");
    strcpy(vgrid[45], "xxxxxxxxxxxxx.llllllllll.......ccc.........cc......lllllllll.......xxxxxxxxxxxxx");
    strcpy(vgrid[46], "xxxxxxxxxx....lllllllllll...4...cc.....2.2.cc....llllllllll.4.......xxxxxxxxxxxx");
    strcpy(vgrid[47], "xxxxxxxxx....4.lllllllllllll....cccccccc=cccc..lllllllllll.....xx....xxxxxxxxxxx");
    strcpy(vgrid[48], "xxxxxxxxxx.....llllllllllllll...cccccccc.cccc..llllllllll......xx....xxxxxxxxxxx");
    strcpy(vgrid[49], "xxxxxxxxxxx.....lllllllllllllll..cc......cc...lllllllllll...........xxxxxxxxxxxx");
    strcpy(vgrid[50], "xxxxxxxxxxx.....llllllllllllll...cc1.....cc.4..lllllllll...........xxxxxxxxxxxxx");
    strcpy(vgrid[51], "xxxxxxxxxxxx.....lllllllllllll...cc......cc....lllllllll.......xx.xxxxxxxxxxxxxx");
    strcpy(vgrid[52], "xxxxxxxxxxxx.......llllllllllll..cccccccccc...lllllllll........xxxxxxxxxxxxxxxxx");
    strcpy(vgrid[53], "xxxxxxxxx.........llllllllllllll.cccccccccc.lllllllllll.......xxxxxxxxxxxxxxxxxx");
    strcpy(vgrid[54], "xxxxxxxxxx....0...llllllllllllll............lllllllll....0....xxxxxxxxxxxxxxxxxx");
    strcpy(vgrid[55], "xxxxxxxxxx.......4.lllllllllllllll..4....lllllllll...........xxxxxxxxxxxxxxxxxxx");
    strcpy(vgrid[56], "xxxxxxxxxxx..........llllllllllllll....lllllll....4.....x........xxxxxxxxxxxxxxx");
    strcpy(vgrid[57], "xxxxxxxxxxx...xx.........lllllllllllllllll...................xx{xxxxxxxxxxxxxxxx");
    strcpy(vgrid[58], "xxxxxxxxxxxxx..xx................lllllll.....................xxxxxxxxxxxxxxxxxxx");
    strcpy(vgrid[59], "xxxxxxxxxxxxxxxxx.........xxx.................xxxxxx......xxxxxxxxxxxxxxxxxxxxxx");
    strcpy(vgrid[60], "xxxxxxxxxxxxxxxxxxx....xxxxxxxx...xxx......xxxxxxxxxx.......xxxxxxxxxxxxxxxxxxxx");
    strcpy(vgrid[61], "xxxxxxxxxxxxxxxxxxxx(xxxxxxxxxxxx[xxxxx...xxxxxxxxxxxxxx...xxxxxxxxxxxxxxxxxxxxx");
    strcpy(vgrid[62], "xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx");
    strcpy(vgrid[63], "xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx");
    strcpy(vgrid[64], "xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx");
    strcpy(vgrid[65], "xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx");
    strcpy(vgrid[66], "xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx");
    strcpy(vgrid[67], "xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx");
    strcpy(vgrid[68], "xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx");
    strcpy(vgrid[69], "xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx");

    mons_array[0] = MONS_ASMODEUS;
    mons_array[1] = MONS_FIEND;
    mons_array[2] = MONS_BALRUG;
    mons_array[3] = MONS_MOLTEN_GARGOYLE;
    mons_array[4] = MONS_SERPENT_OF_HELL;
    mons_array[5] = RANDOM_MONSTER;
    mons_array[6] = RANDOM_MONSTER;

    return MAP_ENCOMPASS;
}

static char antaeus(char vgrid[81][81], FixedVector<int, 7>& mons_array)
{     // bottom of Cocytus. This needs work

    strcpy(vgrid[0], "xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx");
    strcpy(vgrid[1], "xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx");
    strcpy(vgrid[2], "xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx");
    strcpy(vgrid[3], "xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx");
    strcpy(vgrid[4], "xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx");
    strcpy(vgrid[5], "xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx");
    strcpy(vgrid[6], "xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx");
    strcpy(vgrid[7], "xxxxxxxxxxxxxxxxxxxxxxxxxxxx........................xxxxxxxxxxxxxxxxxxxxxxxxxxxx");
    strcpy(vgrid[8], "xxxxxxxxxxxxxxxxxxxxxxxxxxx..........................xxxxxxxxxxxxxxxxxxxxxxxxxxx");
    strcpy(vgrid[9], "xxxxxxxxxxxxxxxxxxxxxxxxxx............................xxxxxxxxxxxxxxxxxxxxxxxxxx");
    strcpy(vgrid[10], "xxxxxxxxxxxxxxxxxxxxxxxxx..............................xxxxxxxxxxxxxxxxxxxxxxxxx");
    strcpy(vgrid[11], "xxxxxxxxxxxxxxxxxxxxxxxx................................xxxxxxxxxxxxxxxxxxxxxxxx");
    strcpy(vgrid[12], "xxxxxxxxxxxxxxxxxxxxxxx....cccccccccccc==cccccccccccc....xxxxxxxxxxxxxxxxxxxxxxx");
    strcpy(vgrid[13], "xxxxxxxxxxxxxxxxxxxxxx....ccccccccccccc==ccccccccccccc....xxxxxxxxxxxxxxxxxxxxxx");
    strcpy(vgrid[14], "xxxxxxxxxxxxxxxxxxxxx....cc............2.............cc....xxxxxxxxxxxxxxxxxxxxx");
    strcpy(vgrid[15], "xxxxxxxxxxxxxxxxxxxx....cc............................cc....xxxxxxxxxxxxxxxxxxxx");
    strcpy(vgrid[16], "xxxxxxxxxxxxxxxxxxx....cc...wwwwwwwwwwwwwwwwwwwwwwww...cc....xxxxxxxxxxxxxxxxxxx");
    strcpy(vgrid[17], "xxxxxxxxxxxxxxxxxx....cc...wwwwwwwwwwwwwwwwwwwwwwwwww...cc....xxxxxxxxxxxxxxxxxx");
    strcpy(vgrid[18], "xxxxxxxxxxxxxxxxx....cc...wwwwwwwwwwwwwwwwwwwwwwwwwwww...cc....xxxxxxxxxxxxxxxxx");
    strcpy(vgrid[19], "xxxxxxxxxxxxxxxx....cc...ww.......3..........3.......ww...cc....xxxxxxxxxxxxxxxx");
    strcpy(vgrid[20], "xxxxxxxxxxxxxxx....cc...ww............................ww...cc....xxxxxxxxxxxxxxx");
    strcpy(vgrid[21], "xxxxxxxxxxxxxx....cc...ww....cccccccccccccccccccccc....ww...cc....xxxxxxxxxxxxxx");
    strcpy(vgrid[22], "xxxxxxxxxxxxx....cc...ww....cccccccccccccccccccccccc....ww...cc....xxxxxxxxxxxxx");
    strcpy(vgrid[23], "xxxxxxxxxxxx....cc...ww....cc......................cc....ww...cc....xxxxxxxxxxxx");
    strcpy(vgrid[24], "xxxxxxxxxxx....cc...ww....cc...T................T...cc....ww...cc....xxxxxxxxxxx");
    strcpy(vgrid[25], "xxxxxxxxxx....cc...ww....cc..........wwwwww..........cc....ww...cc....xxxxxxxxxx");
    strcpy(vgrid[26], "xxxxxxxxx....cc...ww....cc.......wwwwwwwwwwwwww.......cc....ww...cc....xxxxxxxxx");
    strcpy(vgrid[27], "xxxxxxxxx....cc...ww...cc.....wwwwwwwwwwwwwwwwwwww.....cc...ww...cc....xxxxxxxxx");
    strcpy(vgrid[28], "xxxxxxxxx....cc..www..cc....wwwwwwwwwccccccwwwwwwwww....cc..www..cc....xxxxxxxxx");
    strcpy(vgrid[29], "xxxxxxxxx....cc..www.cc....wwwwwwwwccc2|12cccwwwwwwww....cc.www..cc....xxxxxxxxx");
    strcpy(vgrid[30], "xxxxxxxxx....cc..www.cc...wwwwwwwwcc2+....+2ccwwwwwwww...cc.www..cc....xxxxxxxxx");
    strcpy(vgrid[31], "xxxxxxxxx....cc..www.cc...wwwwwwwwcc+cc++cc+ccwwwwwwww...cc.www..cc....xxxxxxxxx");
    strcpy(vgrid[32], "xxxxxxxxx....cc..www..c..wwwwwwwwwc|||c..c$$$cwwwwwwwww..c..www..cc....xxxxxxxxx");
    strcpy(vgrid[33], "xxxxxxxxx....cc..wwww.c.wwwwwwwwwwc|||c..c$$$cwwwwwwwwww.c.wwww..cc....xxxxxxxxx");
    strcpy(vgrid[34], "xxxxxxxxx....cc..wwww.c.wwwwwwwwwwcc||c..c$$ccwwwwwwwwww.c.wwww..cc....xxxxxxxxx");
    strcpy(vgrid[35], "xxxxxxxxx....cc..wwww.c.wwwwwwwwwwwcccc..ccccwwwwwwwwwww.c.wwww..cc....xxxxxxxxx");
    strcpy(vgrid[36], "xxxxxxxxx....cc..www..c..wwwwwwwwwwwwcc==ccwwwwwwwwwwww..c..www..cc....xxxxxxxxx");
    strcpy(vgrid[37], "xxxxxxxxx....cc..www.cc...wwwwwwwwwwwwwwwwwwwwwwwwwwww...cc.www..cc....xxxxxxxxx");
    strcpy(vgrid[38], "xxxxxxxxx....cc..www.cc....wwwwwwwwwwwwwwwwwwwwwwwwwww...cc.www..cc....xxxxxxxxx");
    strcpy(vgrid[39], "xxxxxxxxx....cc..www.cc....wwwwwwwwwwwwwwwwwwwwwwwwww....cc.www..cc....xxxxxxxxx");
    strcpy(vgrid[40], "xxxxxxxxx....cc..www..cc....wwwwwwwwwwwwwwwwwwwwwwww....cc..www..cc....xxxxxxxxx");
    strcpy(vgrid[41], "xxxxxxxxx....cc...ww...cc.....wwwwwwwwwwwwwwwwwwww.....cc...ww...cc....xxxxxxxxx");
    strcpy(vgrid[42], "xxxxxxxxx....cc...ww....cc.......wwwwwwwwwwwwww.......cc....ww...cc....xxxxxxxxx");
    strcpy(vgrid[43], "xxxxxxxxxx....cc...ww....cc..........wwwwww..........cc....ww...cc....xxxxxxxxxx");
    strcpy(vgrid[44], "xxxxxxxxxxx....cc...ww....cc...T................T...cc....ww...cc....xxxxxxxxxxx");
    strcpy(vgrid[45], "xxxxxxxxxxxx....cc...ww....cc...........2..........cc....ww...cc....xxxxxxxxxxxx");
    strcpy(vgrid[46], "xxxxxxxxxxxxx....cc...ww....ccccccccccc==ccccccccccc....ww...cc....xxxxxxxxxxxxx");
    strcpy(vgrid[47], "xxxxxxxxxxxxxx....cc...ww....cccccccccc==cccccccccc....ww...cc....xxxxxxxxxxxxxx");
    strcpy(vgrid[48], "xxxxxxxxxxxxxxx....cc...ww............................ww...cc....xxxxxxxxxxxxxxx");
    strcpy(vgrid[49], "xxxxxxxxxxxxxxxx....cc...ww..........................ww...cc....xxxxxxxxxxxxxxxx");
    strcpy(vgrid[50], "xxxxxxxxxxxxxxxxx....cc...wwwwwwwwwwwwwwwwwwwwwwwwwwww...cc....xxxxxxxxxxxxxxxxx");
    strcpy(vgrid[51], "xxxxxxxxxxxxxxxxxx....cc...wwwwwwwwwwwwwwwwwwwwwwwwww...cc....xxxxxxxxxxxxxxxxxx");
    strcpy(vgrid[52], "xxxxxxxxxxxxxxxxxxx....cc...wwwwwwwwwwwwwwwwwwwwwwww...cc....xxxxxxxxxxxxxxxxxxx");
    strcpy(vgrid[53], "xxxxxxxxxxxxxxxxxxxx....cc............................cc....xxxxxxxxxxxxxxxxxxxx");
    strcpy(vgrid[54], "xxxxxxxxxxxxxxxxxxxxx....cc..........................cc....xxxxxxxxxxxxxxxxxxxxx");
    strcpy(vgrid[55], "xxxxxxxxxxxxxxxxxxxxxx....cccccccccccccccccccccccccccc....xxxxxxxxxxxxxxxxxxxxxx");
    strcpy(vgrid[56], "xxxxxxxxxxxxxxxxxxxxxxx....cccccccccccccccccccccccccc....xxxxxxxxxxxxxxxxxxxxxxx");
    strcpy(vgrid[57], "xxxxxxxxxxxxxxxxxxxxxxxx................................xxxxxxxxxxxxxxxxxxxxxxxx");
    strcpy(vgrid[58], "xxxxxxxxxxxxxxxxxxxxxxxxx..............................xxxxxxxxxxxxxxxxxxxxxxxxx");
    strcpy(vgrid[59], "xxxxxxxxxxxxxxxxxxxxxxxxxx............................xxxxxxxxxxxxxxxxxxxxxxxxxx");
    strcpy(vgrid[60], "xxxxxxxxxxxxxxxxxxxxxxxxxxx..........{.(.[...........xxxxxxxxxxxxxxxxxxxxxxxxxxx");
    strcpy(vgrid[61], "xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx");
    strcpy(vgrid[62], "xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx");
    strcpy(vgrid[63], "xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx");
    strcpy(vgrid[64], "xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx");
    strcpy(vgrid[65], "xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx");
    strcpy(vgrid[66], "xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx");
    strcpy(vgrid[67], "xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx");
    strcpy(vgrid[68], "xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx");
    strcpy(vgrid[69], "xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx");

    mons_array[0] = MONS_ANTAEUS;
    mons_array[1] = MONS_ICE_FIEND;
    mons_array[2] = MONS_ICE_DRAGON;
    mons_array[3] = RANDOM_MONSTER;
    mons_array[4] = RANDOM_MONSTER;
    mons_array[5] = RANDOM_MONSTER;
    mons_array[6] = RANDOM_MONSTER;

    return MAP_ENCOMPASS;
}


static char ereshkigal(char vgrid[81][81], FixedVector<int, 7>& mons_array)
{     // Tartarus
      // note that the tomb on the right isn't supposed to have any doors

    strcpy(vgrid[0],  "xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx");
    strcpy(vgrid[1],  "xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx");
    strcpy(vgrid[2],  "xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx");
    strcpy(vgrid[3],  "xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx");
    strcpy(vgrid[4],  "xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx");
    strcpy(vgrid[5],  "xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx");
    strcpy(vgrid[6],  "xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx");
    strcpy(vgrid[7],  "xxxxxxxxx..............................................................xxxxxxxxx");
    strcpy(vgrid[8],  "xxxxxxxxx..............................................................xxxxxxxxx");
    strcpy(vgrid[9],  "xxxxxxxxx.................cccc..........ccc............................xxxxxxxxx");
    strcpy(vgrid[10], "xxxxxxxxx.............ccccc..cccc.....ccc.cccc.........................xxxxxxxxx");
    strcpy(vgrid[11], "xxxxxxxxx...........ccc.........ccccccc.....cc.........................xxxxxxxxx");
    strcpy(vgrid[12], "xxxxxxxxx.........ccc.......2............V..cc.........................xxxxxxxxx");
    strcpy(vgrid[13], "xxxxxxxxx........cc44.......................cc...........xxxxxxxx......xxxxxxxxx");
    strcpy(vgrid[14], "xxxxxxxxx........cc74ccAAAAcc...............cc..........xx......xx.....xxxxxxxxx");
    strcpy(vgrid[15], "xxxxxxxxx........ccccc......ccc.......ccc++ccc.........xx........xx....xxxxxxxxx");
    strcpy(vgrid[16], "xxxxxxxxx........ccc..........ccc.....ccc44ccc.........x..........x....xxxxxxxxx");
    strcpy(vgrid[17], "xxxxxxxxx........cc............cc....cccc44cc.........xx..........xx...xxxxxxxxx");
    strcpy(vgrid[18], "xxxxxxxxx.......ccc.G........G.ccc7ccc..c44c..........x.....|4.....x...xxxxxxxxx");
    strcpy(vgrid[19], "xxxxxxxxx.......ccc............ccccc..................x.....47.....x...xxxxxxxxx");
    strcpy(vgrid[20], "xxxxxxxxx......cc4..............4cc...................xx..........xx...xxxxxxxxx");
    strcpy(vgrid[21], "xxxxxxxxx......cc4..G........G..4cc..x.........x.......x..........x....xxxxxxxxx");
    strcpy(vgrid[22], "xxxxxxxxx......cc4..............4cc....................xx........xx....xxxxxxxxx");
    strcpy(vgrid[23], "xxxxxxxxx.......ccc............ccc......................xx......xx.....xxxxxxxxx");
    strcpy(vgrid[24], "xxxxxxxxx.......ccc.F........F.ccc.......................xxxxxxxx......xxxxxxxxx");
    strcpy(vgrid[25], "xxxxxxxxx........cc4..........4cc......................................xxxxxxxxx");
    strcpy(vgrid[26], "xxxxxxxxx........ccc....17....ccc....x.........x.......................xxxxxxxxx");
    strcpy(vgrid[27], "xxxxxxxxx........ccccc......ccccc......................................xxxxxxxxx");
    strcpy(vgrid[28], "xxxxxxxxx........ccccBccccccBcccc......................................xxxxxxxxx");
    strcpy(vgrid[29], "xxxxxxxxx........cc**cccccccc**cc......................................xxxxxxxxx");
    strcpy(vgrid[30], "xxxxxxxxx.........cc**||||||**cc.......................................xxxxxxxxx");
    strcpy(vgrid[31], "xxxxxxxxx..........cccccccccccc......x.........x............V..........xxxxxxxxx");
    strcpy(vgrid[32], "xxxxxxxxx..............................................................xxxxxxxxx");
    strcpy(vgrid[33], "xxxxxxxxx...........................................cccccccccccccccc...xxxxxxxxx");
    strcpy(vgrid[34], "xxxxxxxxx...........................................cccccccccccccccc...xxxxxxxxx");
    strcpy(vgrid[35], "xxxxxxxxx...........................................cc$$$$xxx|||||cc...xxxxxxxxx");
    strcpy(vgrid[36], "xxxxxxxxx.......V........V...........x.........x....cc$$$$xxx|||||cc...xxxxxxxxx");
    strcpy(vgrid[37], "xxxxxxxxx...........................................ccxxxxxccccccccc...xxxxxxxxx");
    strcpy(vgrid[38], "xxxxxxxxx...........................................ccxxxxxccccccccc...xxxxxxxxx");
    strcpy(vgrid[39], "xxxxxxxxx...........................................cc44744xx22222cc...xxxxxxxxx");
    strcpy(vgrid[40], "xxxxxxxxx.......cCCCCCCCCCCCCCCCCCc.................cc44444xx22222cc...xxxxxxxxx");
    strcpy(vgrid[41], "xxxxxxxxx.......c3.2..........3...c..x.........x...cccccccccccxxxxcc...xxxxxxxxx");
    strcpy(vgrid[42], "xxxxxxxxx.......c.x.x.x.x.x.x.x.x.c.................ccccccccccxxxxcc...xxxxxxxxx");
    strcpy(vgrid[43], "xxxxxxxxx.......c...2.3..4..5..4..c......................=.......xcc...xxxxxxxxx");
    strcpy(vgrid[44], "xxxxxxxxx.......cx.x.x.x.x.x.x.x.xc......................=.......xcc...xxxxxxxxx");
    strcpy(vgrid[45], "xxxxxxxxx.......c..65..3..6.6...5.c.................ccccccccccxxxxcc...xxxxxxxxx");
    strcpy(vgrid[46], "xxxxxxxxx.......c.x.x.x.x.x.x.x.x.c..x.........x...cccccccccccxxxxcc...xxxxxxxxx");
    strcpy(vgrid[47], "xxxxxxxxx.......c...4...3.....4...c.................cc34343xx555555c...xxxxxxxxx");
    strcpy(vgrid[48], "xxxxxxxxx.......ccccccccDDDDccccccc.................cc43434xx525525c...xxxxxxxxx");
    strcpy(vgrid[49], "xxxxxxxxx.......c$$$$$$E.25.H$$$||c.................ccxxxxxccccccccc...xxxxxxxxx");
    strcpy(vgrid[50], "xxxxxxxxx.......c$c$$c$E.cc.H$c$c|c.................ccxxxxxccccccccc...xxxxxxxxx");
    strcpy(vgrid[51], "xxxxxxxxx.......c||||||E.556H$$$||c..x.........x....cc$$xx56565xx$|c...xxxxxxxxx");
    strcpy(vgrid[52], "xxxxxxxxx.......ccccccccccccccccccc.................cc$$xx65656xx|7c...xxxxxxxxx");
    strcpy(vgrid[53], "xxxxxxxxx...........................................cccccccccccccccc...xxxxxxxxx");
    strcpy(vgrid[54], "xxxxxxxxx...........................................cccccccccccccccc...xxxxxxxxx");
    strcpy(vgrid[55], "xxxxxxxxx..............................................................xxxxxxxxx");
    strcpy(vgrid[56], "xxxxxxxxx..............................................................xxxxxxxxx");
    strcpy(vgrid[57], "xxxxxxxxx........(...........................................[.........xxxxxxxxx");
    strcpy(vgrid[58], "xxxxxxxxx..............................................................xxxxxxxxx");
    strcpy(vgrid[59], "xxxxxxxxx..............................................................xxxxxxxxx");
    strcpy(vgrid[60], "xxxxxxxxx..............................{...............................xxxxxxxxx");
    strcpy(vgrid[61], "xxxxxxxxx..............................................................xxxxxxxxx");
    strcpy(vgrid[62], "xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx");
    strcpy(vgrid[63], "xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx");
    strcpy(vgrid[64], "xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx");
    strcpy(vgrid[65], "xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx");
    strcpy(vgrid[66], "xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx");
    strcpy(vgrid[67], "xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx");
    strcpy(vgrid[68], "xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx");
    strcpy(vgrid[69], "xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx");

    mons_array[0] = MONS_ERESHKIGAL;
    mons_array[1] = MONS_NECROPHAGE;
    mons_array[2] = MONS_WRAITH;
    mons_array[3] = MONS_SHADOW;
    mons_array[4] = MONS_ZOMBIE_SMALL;
    mons_array[5] = MONS_SKELETON_SMALL;
    mons_array[6] = MONS_SHADOW_FIEND;

    replace_one_of( vgrid, 80, 70, 'A', '=', 'c' );
    replace_one_of( vgrid, 80, 70, 'B', '=', 'c' );
    replace_one_of( vgrid, 80, 70, 'C', '=', 'c' );
    replace_one_of( vgrid, 80, 70, 'D', '=', 'c' );
    replace_one_of( vgrid, 80, 70, 'E', '=', 'c' );
    replace_one_of( vgrid, 80, 70, 'H', '=', 'c' );

    return MAP_ENCOMPASS;
}


static char mnoleg(char vgrid[81][81], FixedVector<int, 7>& mons_array)
{

    for (unsigned char i = 0; i < 81; i++)
        strcpy(vgrid[i], "xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx");

    strcat(vgrid[0],  "xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx");
    strcat(vgrid[1],  "xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx");
    strcat(vgrid[2],  "xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx");
    strcat(vgrid[3],  "xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx");
    strcat(vgrid[4],  "xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx");
    strcat(vgrid[5],  "xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx");
    strcat(vgrid[6],  "xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx");
    strcat(vgrid[7],  "x.................2............xxxxxxxxx");
    strcat(vgrid[8],  "x.....2........................xxxxxxxxx");
    strcat(vgrid[9],  "x..cccccccc...ccccccc..ccccccc.xxxxxxxxx");
    strcat(vgrid[10], "x..ccccccccc.2.ccccccc..cccccc.xxxxxxxxx");
    strcat(vgrid[11], "x..cccccccccc...ccccccc..ccccc.xxxxxxxxx");
    strcat(vgrid[12], "x..ccccccccccc.1.ccccccc..cccc.xxxxxxxxx");
    strcat(vgrid[13], "x2.cccccccccc.2..cccccccc2.ccc.xxxxxxxxx");
    strcat(vgrid[14], "x..ccccccccc.....ccccccccc..cc.xxxxxxxxx");
    strcat(vgrid[15], "x..cccccccc...c...ccccccccc..c.xxxxxxxxx");
    strcat(vgrid[16], "x..ccccccc...ccc...ccccccccc...xxxxxxxxx");
    strcat(vgrid[17], "x..cccccc...ccccc...ccccccccc..xxxxxxxxx");
    strcat(vgrid[18], "x..ccccc...ccccccc...ccccccccc.xxxxxxxxx");
    strcat(vgrid[19], "x..cccc...ccccccccc...ccccccc..xxxxxxxxx");
    strcat(vgrid[20], "x..ccc.2.ccccccccccc.2.ccccc...xxxxxxxxx");
    strcat(vgrid[21], "x..cc.....ccccccccccc...ccc....xxxxxxxxx");
    strcat(vgrid[22], "x..c...c...ccccccccccc...c.2...xxxxxxxxx");
    strcat(vgrid[23], "x.....ccc.2.ccccccccccc......c.xxxxxxxxx");
    strcat(vgrid[24], "x....ccccc...ccccccccccc....cc.xxxxxxxxx");
    strcat(vgrid[25], "x.2.ccccccc...ccccccccccc..ccc.xxxxxxxxx");
    strcat(vgrid[26], "x.................2.......cccc.xxxxxxxxx");
    strcat(vgrid[27], "x...c..ccccccc.ccccccc...ccccc.xxxxxxxxx");
    strcat(vgrid[28], "x..ccc......2c.c2cccc...cccccc.xxxxxxxxx");
    strcat(vgrid[29], "x.ccccc..ccc.c.c2ccc.2.ccccccc.xxxxxxxxx");
    strcat(vgrid[30], "x.cccccc..cc.c.c.cc...cccccccc.xxxxxxxxx");
    strcat(vgrid[31], "x.ccccccc..c.c.c.c...ccccccccc.xxxxxxxxx");
    strcat(vgrid[32], "x.cccccccc...c.c....cccccccccc.xxxxxxxxx");
    strcat(vgrid[33], "x.ccccccccc..c.c...ccccccccccc.xxxxxxxxx");
    strcat(vgrid[34], "x..............................xxxxxxxxx");
    strcat(vgrid[35], "xxxxxxxxxxxxxx@xxxxxxxxxxxxxxxxxxxxxxxxx");

    mons_array[0] = MONS_MNOLEG;
    mons_array[1] = MONS_NEQOXEC;
    mons_array[2] = RANDOM_MONSTER;
    mons_array[3] = RANDOM_MONSTER;
    mons_array[4] = RANDOM_MONSTER;
    mons_array[5] = RANDOM_MONSTER;
    mons_array[6] = RANDOM_MONSTER;

    return MAP_NORTHEAST;
}


static char lom_lobon(char vgrid[81][81], FixedVector<int, 7>& mons_array)
{

    strcpy(vgrid[0], "xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx");
    strcpy(vgrid[1], "xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx");
    strcpy(vgrid[2], "xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx");
    strcpy(vgrid[3], "xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx");
    strcpy(vgrid[4], "xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx");
    strcpy(vgrid[5], "xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx");
    strcpy(vgrid[6], "xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx");
    strcpy(vgrid[7], "xxxxxxxxxxxxxxxxxxxxxxxxxxxwwwwwwwwwwww.......wwwwwwwxxxxxxxxxxxxxxxxxxxxxxxxxxx");
    strcpy(vgrid[8], "xxxxxxxxxxxxxxxxxxxxxxwwwwwwwwwwwwbbbwwwwwww.......wwwwwwwxxxxxxxxxxxxxxxxxxxxxx");
    strcpy(vgrid[9], "xxxxxxxxxxxxxxxxxxwwwwwwwwwwwwbbbbbbbbbbbwwwwww.........wwwwwwxxxxxxxxxxxxxxxxxx");
    strcpy(vgrid[10], "xxxxxxxxxxxxxxxwwwwwwwwwwwwbbbbwwwwwwwwwbbbbwwwwww.........wwwwwwxxxxxxxxxxxxxxx");
    strcpy(vgrid[11], "xxxxxxxxxxxxxwwwwwwwbbbbbbbbwwwwwwwwwwwwwwwbbbwwwww...........wwwwwxxxxxxxxxxxxx");
    strcpy(vgrid[12], "xxxxxxxxxxxxwwwwwbbbb......bbbwwwwwwwwwwww...bbwwwww......3......wwwxxxxxxxxxxxx");
    strcpy(vgrid[13], "xxxxxxxxxxxxwwwbbb...........bbbwwwwww........bbwwwww.............wwxxxxxxxxxxxx");
    strcpy(vgrid[14], "xxxxxxxxxxxwwwbb...............bbwwww..........bwwwwww.............wwxxxxxxxxxxx");
    strcpy(vgrid[15], "xxxxxxxxxxxwwbb........1........bbww...........bbwwww..............wwxxxxxxxxxxx");
    strcpy(vgrid[16], "xxxxxxxxxxwwwb...................bw......2......bwww.....U....2.....wwxxxxxxxxxx");
    strcpy(vgrid[17], "xxxxxxxxxxwwbb........444........bb.............bww.................wwxxxxxxxxxx");
    strcpy(vgrid[18], "xxxxxxxxxxwwbb..3.............3..bbA............bbw..............4..wwxxxxxxxxxx");
    strcpy(vgrid[19], "xxxxxxxxxwwbbb...................b.A............4=...................wwxxxxxxxxx");
    strcpy(vgrid[20], "xxxxxxxxxwwbwbb.................bb.A4....U......4=.........U..........wxxxxxxxxx");
    strcpy(vgrid[21], "xxxxxxxxxwwbwwbb...............bb..A............bbw..............4.....xxxxxxxxx");
    strcpy(vgrid[22], "xxxxxxxxxwwbbwwbbb...........bbb..AA............bwww...................xxxxxxxxx");
    strcpy(vgrid[23], "xxxxxxxxxwwwbwwwwBBBb..2..bbbb....A.............bwww...................xxxxxxxxx");
    strcpy(vgrid[24], "xxxxxxxxxxwwbwwww..bBBBBBBb.......Aw.....3.....bbwwww...U.....3.......xxxxxxxxxx");
    strcpy(vgrid[25], "xxxxxxxxxxwwbbww.................bbww........wwbwwwww.................xxxxxxxxxx");
    strcpy(vgrid[26], "xxxxxxxxxxwwwbbw................bbwwwww....wwwbbwwww..................xxxxxxxxxx");
    strcpy(vgrid[27], "xxxxxxxxxxwwwwbb...4...U...3....bwwwwwwwwwwwwbbwww....................xxxxxxxxxx");
    strcpy(vgrid[28], "xxxxxxxxxxxwwwwbbb...........bbbbbwwwwwwwwwbbbwww....................xxxxxxxxxxx");
    strcpy(vgrid[29], "xxxxxxxxxxxwwwwwwbbbb.....bbbbwwwbbbbwwwbbbbwwww....................xxxxxxxxxxxx");
    strcpy(vgrid[30], "xxxxxxxxxxxwwwwwwwwwbbbbbbbwwwwwwwwwbbbbbwwwww......4.....4........xxxxxxxxxxxxx");
    strcpy(vgrid[31], "xxxxxxxxxxxxwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwww......................xxxxxxxxxxxxxx");
    strcpy(vgrid[32], "xxxxxxxxxxxxwwwwwwwwwwwwwwwwwwwwwwwwwwwww.......................xxxxxxxxxxxxxxxx");
    strcpy(vgrid[33], "xxxxxxxxxxxxxxwwwwwwwwwwwwwwwwwwwww........................xxxxxxxxxxxxxxxxxxxxx");
    strcpy(vgrid[34], "xxxxxxxxxxxxxxxxxxxxxxxxxwwwwwww......................xxxxxxxxxxxxxxxxxxxxxxxxxx");
    strcpy(vgrid[35], "xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx...@.xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx");

    mons_array[0] = MONS_LOM_LOBON;
    mons_array[1] = MONS_GIANT_ORANGE_BRAIN;
    mons_array[2] = MONS_RAKSHASA;
    mons_array[3] = MONS_WIZARD;
    mons_array[4] = RANDOM_MONSTER;
    mons_array[5] = RANDOM_MONSTER;
    mons_array[6] = RANDOM_MONSTER;

    replace_one_of( vgrid, 40, 30, 'A', '=', 'b' );
    replace_one_of( vgrid, 40, 30, 'B', '=', 'b' );

    return MAP_NORTH;
}


static char cerebov(char vgrid[81][81], FixedVector<int, 7>& mons_array)
{     // you might not want to teleport too much on this level -
      // unless you can reliably teleport away again.

    for (unsigned char i = 0; i < 81; i++)
        strcpy(vgrid[i], "xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx");

    strcat(vgrid[0],  "xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx");
    strcat(vgrid[1],  "xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx");
    strcat(vgrid[2],  "xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx");
    strcat(vgrid[3],  "xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx");
    strcat(vgrid[4],  "xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx");
    strcat(vgrid[5],  "xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx");
    strcat(vgrid[6],  "xxxxxxxxxxxxxllllxxxxxxxxxxxxxxxxxxxxxxx");
    strcat(vgrid[7],  "............lllllll.....lll....xxxxxxxxx");
    strcat(vgrid[8],  "...l..l....llvvvvvlll...l.lll..xxxxxxxxx");
    strcat(vgrid[9],  ".ll..lll..ll.v$$$vl.l..........xxxxxxxxx");
    strcat(vgrid[10], "llllll4l.ll..v|||v..ll..4lll.llxxxxxxxxx");
    strcat(vgrid[11], "l..l...lll...v$$$v...lllll.llllxxxxxxxxx");
    strcat(vgrid[12], "lvvvvv...vvvvvvvvvvvvv.llvvvvvllxxxxxxxx");
    strcat(vgrid[13], "lv|$|vvvvv.....4.....vvvvv$|$vllxxxxxxxx");
    strcat(vgrid[14], "lv$|$v.....vvvvvvvvv.....v|$|vlxxxxxxxxx");
    strcat(vgrid[15], "lv|$|v.vvvvvvvv|vvvvvvvv.v$|$vlxxxxxxxxx");
    strcat(vgrid[16], "lvvvvv.vvvvvv.***.vvvvvv.vvvvvlxxxxxxxxx");
    strcat(vgrid[17], "l..v...vv.....vvv.....vv...v.llxxxxxxxxx");
    strcat(vgrid[18], "l4.v.vvvv....vv1vv....vvvv.v.l.xxxxxxxxx");
    strcat(vgrid[19], "ll.v4vv......v...v......vv4vll.xxxxxxxxx");
    strcat(vgrid[20], "ll.v.vvvv......3......vvvv.vl4.xxxxxxxxx");
    strcat(vgrid[21], "l..v...vv..2.......2..vv...vllllxxxxxxxx");
    strcat(vgrid[22], "lvvvvv.vv..2.......2..vv.vvvvvllxxxxxxxx");
    strcat(vgrid[23], "lv|$|v.vv.............vv.v$|$vlxxxxxxxxx");
    strcat(vgrid[24], "lv|$|v.vv...vv...vv...vv.v$|$vlxxxxxxxxx");
    strcat(vgrid[25], "lv|$|v.vv...vv=v=vv...vv.v$|$vllxxxxxxxx");
    strcat(vgrid[26], "lvvvvv.vvvvvvv...vvvvvvv.vvvvvllxxxxxxxx");
    strcat(vgrid[27], "ll..v..vvvvvvv...vvvvvvv..vllllxxxxxxxxx");
    strcat(vgrid[28], "ll4.vv......=.....=......vvl.llxxxxxxxxx");
    strcat(vgrid[29], ".ll..vv.vvvvv..2..vvvvv.vvll...xxxxxxxxx");
    strcat(vgrid[30], "..lll.vvv|||v.....v$$$vvvlll...xxxxxxxxx");
    strcat(vgrid[31], "....l...v|$|vv.l.vv$|$vlll.4...xxxxxxxxx");
    strcat(vgrid[32], "...llll.v|||vlllllv$$$vll....l.xxxxxxxxx");
    strcat(vgrid[33], "...ll.llvvvvvlllllvvvvvl....ll.xxxxxxxxx");
    strcat(vgrid[34], ".l.....lll.lllll.llllllll...l..xxxxxxxxx");
    strcat(vgrid[35], ".........lll...@...ll..l.......xxxxxxxxx");

    mons_array[0] = MONS_CEREBOV;
    mons_array[1] = MONS_BALRUG;
    mons_array[2] = MONS_PIT_FIEND;
    mons_array[3] = MONS_FIRE_VORTEX;
    mons_array[4] = RANDOM_MONSTER;
    mons_array[5] = RANDOM_MONSTER;
    mons_array[6] = RANDOM_MONSTER;

    return MAP_NORTHEAST;
}


static char gloorx_vloq(char vgrid[81][81], FixedVector<int, 7>& mons_array)
{

    for (unsigned char i = 0; i < 81; i++)
        strcpy(vgrid[i], "xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx");

    strcpy(vgrid[36], "xxxxxxxxxxxxxxxxxxxxxxx@.xxxxxxxxxxxxxxx");
    strcpy(vgrid[37], "xxxxxxxxx..............................x");
    strcpy(vgrid[38], "xxxxxxxxx..............................x");
    strcpy(vgrid[39], "xxxxxxxxx..............................x");
    strcpy(vgrid[40], "xxxxxxxxx.v.v.v.v.v.v.v..v.v.v.v.v.v.v.x");
    strcpy(vgrid[41], "xxxxxxxxx..............................x");
    strcpy(vgrid[42], "xxxxxxxxx.v.ccAAAAAAAAAAAAAAAAAAAAcc.v.x");
    strcpy(vgrid[43], "xxxxxxxxx...cc....................cc...x");
    strcpy(vgrid[44], "xxxxxxxxx.v.A..ccccc..4..4..ccccc..A.v.x");
    strcpy(vgrid[45], "xxxxxxxxx...A.cc.3............3.cc.A...x");
    strcpy(vgrid[46], "xxxxxxxxx.v.A.c..ccccc.cc.ccccc..c.A.v.x");
    strcpy(vgrid[47], "xxxxxxxxx...A.c.cc.....cc.....cc.c.A...x");
    strcpy(vgrid[48], "xxxxxxxxx.v.A.c.c.2...cccc...2.c.c.A.v.x");
    strcpy(vgrid[49], "xxxxxxxxx...A...c...ccc..ccc...c...A...x");
    strcpy(vgrid[50], "xxxxxxxxx.v.A.3.=...=2.13.2=...=.3.A.v.x");
    strcpy(vgrid[51], "xxxxxxxxx...A...c...ccc..ccc...c...A...x");
    strcpy(vgrid[52], "xxxxxxxxx.v.A.c.c.2...cccc...2.c.c.A.v.x");
    strcpy(vgrid[53], "xxxxxxxxx...A.c.cc.....cc.....cc.c.A...x");
    strcpy(vgrid[54], "xxxxxxxxx.v.A.c..ccccc.cc.ccccc..c.A.v.x");
    strcpy(vgrid[55], "xxxxxxxxx...A.cc.3............3.cc.A...x");
    strcpy(vgrid[56], "xxxxxxxxx.v.A..ccccc..4..4..ccccc..A.v.x");
    strcpy(vgrid[57], "xxxxxxxxx...cc....................cc...x");
    strcpy(vgrid[58], "xxxxxxxxx.v.ccAAAAAAAAAAAAAAAAAAAAcc.v.x");
    strcpy(vgrid[59], "xxxxxxxxx..............................x");
    strcpy(vgrid[60], "xxxxxxxxx.v.v.v.v.v.v.v..v.v.v.v.v.v.v.x");
    strcpy(vgrid[61], "xxxxxxxxx..............................x");
    strcpy(vgrid[62], "xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx");
    strcpy(vgrid[63], "xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx");
    strcpy(vgrid[64], "xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx");
    strcpy(vgrid[65], "xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx");
    strcpy(vgrid[66], "xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx");
    strcpy(vgrid[67], "xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx");
    strcpy(vgrid[68], "xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx");
    strcpy(vgrid[69], "xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx");

    mons_array[0] = MONS_GLOORX_VLOQ;
    mons_array[1] = MONS_EXECUTIONER;
    mons_array[2] = MONS_DEMONIC_CRAWLER;
    mons_array[3] = MONS_SHADOW_DEMON;
    mons_array[4] = RANDOM_MONSTER;
    mons_array[5] = RANDOM_MONSTER;
    mons_array[6] = RANDOM_MONSTER;

    replace_one_of( vgrid, 40, 60, 'A', '=', 'c' );

    return MAP_SOUTHWEST;
}


static char beehive(char vgrid[81][81], FixedVector<int, 7>& mons_array)
{

    strcpy(vgrid[0],  "xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx");
    strcpy(vgrid[1],  "xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx");
    strcpy(vgrid[2],  "xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx");
    strcpy(vgrid[3],  "xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx");
    strcpy(vgrid[4],  "xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx");
    strcpy(vgrid[5],  "xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx");
    strcpy(vgrid[6],  "xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx");
    strcpy(vgrid[7],  "xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx");
    strcpy(vgrid[8],  "xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx");
    strcpy(vgrid[9],  "xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxaaaaaaaaaxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx");
    strcpy(vgrid[10], "xxxxxxxxxxxxxxxxxxxxxxaaaaaaaaaaaRaaaaaaaaaxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx");
    strcpy(vgrid[11], "xxxxxxxxxxxxxxxxxaaaaaaaaaaRa2aaR1RaaRa2aaaaaaxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx");
    strcpy(vgrid[12], "xxxxxxxxxxxxxxxxxxaaaaaaaaaaRa2a3R3aRaRaRaaaaaaaaaxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx");
    strcpy(vgrid[13], "xxxxxxxxxxxxxxxxxxxxxaaaaRaRaRaaa3aaa3aRa.a.aaaaaaaaaaaxxxxxxxxxxxxxxxxxxxxxxxxx");
    strcpy(vgrid[14], "xxxxxxxxxxxxxxxxaaaaaaRa.aRa2a2a2a2aRaRa.a.a3aaaaaaaaaaaaaaxxxxxxxxxxxxxxxxxxxxx");
    strcpy(vgrid[15], "xxxxxxxxxxxxx4aaaaaaaaa.aaRaRaa2aa2aaRaaa.aa3a33aaaaaaaaaa.44xxxxxxxxxxxxxxxxxxx");
    strcpy(vgrid[16], "xxxxxxxxxxx.4aaaaaaa.222a3a.aaaRaaa.aaa.R3aa3a3aaaaaaaa.....4xxxxxxxxxxxxxxxxxxx");
    strcpy(vgrid[17], "xxxxxxxxxx....aaaaaaa.aRa.a3aRaRa.a3a.a.a.a.aRa2aaaaaa....xxxxxxxxxxxxxxxxxxxxxx");
    strcpy(vgrid[18], "xxxxxxxxxxx...aaaaaa3a3a.a.a.a3aRa2aRa3a.a.aRaRa.aaaaa...xxxxxxxxxxxxxxxxxxxxxxx");
    strcpy(vgrid[19], "xxxxxxxxxxxx...aa2aRa3a3a3aRa.a3a.a.a.a.a.a.a.a3a.aaa...xxxxxxxxxxxxxxxxxxxxxxxx");
    strcpy(vgrid[20], "xxxxxxxxxxxx...aaa.a.a.a2a.aaa.aRaRa2a.a2a3a.a2aaaa..T..xxxxxxxxxxxxxxxxxxxxxxxx");
    strcpy(vgrid[21], "xxxxxxxxxxx.....a2a.a2a.aRaaaaa3a.a.aaa3a3a3a3a.a.........xxxxxxxxxxxxxxxxxxxxxx");
    strcpy(vgrid[22], "xxxxxxxxxxx.4...aaRRaa.a2a.a3a3a3a.aaa.a.aRa.a.aa..4.......xxxxxxxxxxxxxxxxxxxxx");
    strcpy(vgrid[23], "xxxxxxxxxx......a.a.aaa.a3a.a.a.a.aaa2a.a2a.a.aRaa.....4...xxxxxxxxxxxxxxxxxxxxx");
    strcpy(vgrid[24], "xxxxxxxxxx.....aa3a2aaa.a.a.a3a3a3a3aRaaa.a2a.a2aa........xxxxxxxxxxxxxxxxxxxxxx");
    strcpy(vgrid[25], "xxxxxxxxxxx...aaaa.a2aRa.a.a2aaa.a.a.a.aaa.a.aaaa.....xxxxxxxxxxxxxxxxxxxxxxxxxx");
    strcpy(vgrid[26], "xxxxxxxxxxxx..aaa.a.a.a.a.a.a.aaa2a.a3a2a.a2aaa.....xxxxxxxxxxxxxxxxxxxxxxxxxxxx");
    strcpy(vgrid[27], "xxxxxxxxxxxxxx.aaaa3a.a2aRa.a.aaaRa.a.aa.a.aaa....xxxxxxxxxxxxxxxxxxxxxxxxxxxxxx");
    strcpy(vgrid[28], "xxxxxxxxxxxxxxx...aaaaRa.a3a3a.a.a.aaa.aa.aa....4xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx");
    strcpy(vgrid[29], "xxxxxxxxxxxxx........aa.a2a.a.aaa2aa.aa.aaa....xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx");
    strcpy(vgrid[30], "xxxxxxxxxxxxx....4.....a.a2a2a.a2a.a2a.......4.xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx");
    strcpy(vgrid[31], "xxxxxxxxxxx.............a.a.a.a.a.a.....4....xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx");
    strcpy(vgrid[32], "xxxxxxxxxx..............4..a.a.a......4...xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx");
    strcpy(vgrid[33], "xxxxxxxxxx.................a.a.........xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx");
    strcpy(vgrid[34], "xxxxxxxxxxx........................xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx");
    strcpy(vgrid[35], "xxxxxxxxxxx.....4...T............xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx");
    strcpy(vgrid[36], "xxxxxxxxxxx.......................xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx");
    strcpy(vgrid[37], "xxxxxxxxxxxx.........................xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx");
    strcpy(vgrid[38], "xxxxxxxxxxxx.................T.........xxxxxxxx..xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx");
    strcpy(vgrid[39], "xxxxxxxxxxxx.......4.....................xxxxxxx...xxxxxxxxxxxxxxxxxxxxxxxxxxxxx");
    strcpy(vgrid[40], "xxxxxxxxxxx..............xx...............xxxxxx....xxxxxxxxxxxxxxxxxxxxxxxxxxxx");
    strcpy(vgrid[41], "xxxxxxxxxxx............xxxxx........4......xxxx..4....xxxxxxxxxxxxxxxxxxxxxxxxxx");
    strcpy(vgrid[42], "xxxxxxxxxxx..T..........xxx................xxxxx...T.xxxxxxxxxxxxxxxxxxxxxxxxxxx");
    strcpy(vgrid[43], "xxxxxxxxxxx............xxx........T.........xxx........xxxxxxxxxxxxxxxxxxxxxxxxx");
    strcpy(vgrid[44], "xxxxxxxxxxx....4........xx....................x..........xxxxxxxxxxxxxxxxxxxxxxx");
    strcpy(vgrid[45], "xxxxxxxxxx...............x.x...xxx...............xx.xxxxxxxxxxxxxxxxxxxxxxxxxxxx");
    strcpy(vgrid[46], "xxxxxxxxxx.........4...........xxx..................xxxxxxxxxxxxxxxxxxaaaaaxxxxx");
    strcpy(vgrid[47], "xxxxxxxxx.....4.....................4......4...........4...xxxxxxxxxxaa5a5aaxxxx");
    strcpy(vgrid[48], "xxxxxxxxx.................................................wwwwwwwwxxaa5***5axxxx");
    strcpy(vgrid[49], "xxxxxxxxx............x...x...T.....xxxx.................wwwwwwwwwwwwaAa***aaxxxx");
    strcpy(vgrid[50], "xxxxxxxxxx.........xx.............xxxxx................wwwwwwwwwwwwwwaAa5aaxxxxx");
    strcpy(vgrid[51], "xxxxxxxxxxx.......x..................xxx....4..........wwwwwwwwwwwwwwwaA5aaxxxxx");
    strcpy(vgrid[52], "xxxxxxxxxxx.....xxx...4...........................xxxx.4wwwwwwwwwwwwwwwAAaxxxxxx");
    strcpy(vgrid[53], "xxxxxxxxxxxx..xxx.............xx....(.........xxxxxxxx....wwwwwwwwwwwwwwaaxxxxxx");
    strcpy(vgrid[54], "xxxxxxxxxxxxxxxx.............xxxx..................xxxx......wwwwwwwwwwxxxxxxxxx");
    strcpy(vgrid[55], "xxxxxxxxxxxxxxxxx....{..}..xxxxxx..]......xxx...........4.wwwwwwwwwwwwxxxxxxxxxx");
    strcpy(vgrid[56], "xxxxxxxxxxxxxxxxxxxx........xxx........xxxxxx....4....wwwwwwwwwwwwwwxxxxxxxxxxxx");
    strcpy(vgrid[57], "xxxxxxxxxxxxxxxxxxxxxxxxx..[.xxx........xxx)....wwwwwwwwwwwwwwwwwwxxxxxxxxxxxxxx");
    strcpy(vgrid[58], "xxxxxxxxxxxxxxxxxxxxxxxxxxxx.........xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx");
    strcpy(vgrid[59], "xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx");
    strcpy(vgrid[60], "xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx");
    strcpy(vgrid[61], "xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx");
    strcpy(vgrid[62], "xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx");
    strcpy(vgrid[63], "xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx");
    strcpy(vgrid[64], "xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx");
    strcpy(vgrid[65], "xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx");
    strcpy(vgrid[66], "xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx");
    strcpy(vgrid[67], "xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx");
    strcpy(vgrid[68], "xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx");
    strcpy(vgrid[69], "xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx");

    mons_array[0] = MONS_QUEEN_BEE;
    mons_array[1] = MONS_KILLER_BEE;
    mons_array[2] = MONS_KILLER_BEE_LARVA;
    mons_array[3] = MONS_PLANT;
    mons_array[4] = MONS_YELLOW_WASP;
    mons_array[5] = RANDOM_MONSTER;
    mons_array[6] = RANDOM_MONSTER;

    replace_one_of( vgrid, 80, 60, 'A', '=', 'a' );
    replace_one_of( vgrid, 80, 60, '*', '|', '*' );

    return MAP_ENCOMPASS;
}


static char vaults_vault(char vgrid[81][81], FixedVector<int, 7>& mons_array)
{     // last level of the vaults -- dungeon.cc will change all these 'x's

    strcpy(vgrid[0],  "xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx");
    strcpy(vgrid[1],  "xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx");
    strcpy(vgrid[2],  "xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx");
    strcpy(vgrid[3],  "xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx");
    strcpy(vgrid[4],  "xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx");
    strcpy(vgrid[5],  "xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx");
    strcpy(vgrid[6],  "xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx");
    strcpy(vgrid[7],  "xxxxxxxxx..............................................................xxxxxxxxx");
    strcpy(vgrid[8],  "xxxxxxxxx..............................................................xxxxxxxxx");
    strcpy(vgrid[9],  "xxxxxxxxx..xxxxxxxxxxxxxxxxxxxxxxxxxxx....xxxxxxxxxxxxxxxxxxxxxxxxxxx..xxxxxxxxx");
    strcpy(vgrid[10], "xxxxxxxxx..x9.......................9x....xxxxxxxxxxxxxxxxxxxxxxxxxxx..xxxxxxxxx");
    strcpy(vgrid[11], "xxxxxxxxx..x.xxxxxxxxxxxxxxxxxxxxxxx.x....xxxxx.................xxxxx..xxxxxxxxx");
    strcpy(vgrid[12], "xxxxxxxxx..x.x*.*.*.*.*x99x.......8x.x....xxx..........8..........xxx..xxxxxxxxx");
    strcpy(vgrid[13], "xxxxxxxxx..x.x.*b*.*b*.x..x..xx=x..x.x....xxx.....................xxx..xxxxxxxxx");
    strcpy(vgrid[14], "xxxxxxxxx..x.x*.*.*.*.*=..x..x||x..x.x....xx......9........9.......xx..xxxxxxxxx");
    strcpy(vgrid[15], "xxxxxxxxx..x.x.*.*b*.*.x..x..x||x..x.x....xx.......................xx..xxxxxxxxx");
    strcpy(vgrid[16], "xxxxxxxxx..x.x*.*.*.*.*x..=..x||x..x.x....xx......xxxxxxxxxxx......xx..xxxxxxxxx");
    strcpy(vgrid[17], "xxxxxxxxx..x.x.*b*.*b*.x..x..xxxx..x.x....xx......x.........x......xx..xxxxxxxxx");
    strcpy(vgrid[18], "xxxxxxxxx..x.x*.*.*.*.*x88x........x.x....xx....xxx.........=xx....xx..xxxxxxxxx");
    strcpy(vgrid[19], "xxxxxxxxx..x.x.*.*.*.*.xxxxxxxx=xxxx.x....xx.9..x..8.xxxxx.9..x..8.xx..xxxxxxxxx");
    strcpy(vgrid[20], "xxxxxxxxx..x.xxxxxxxxxxx9998......9x8x....xx....x...xx$$$xx...x....xx..xxxxxxxxx");
    strcpy(vgrid[21], "xxxxxxxxx..x.=.........899xxxxxxxxxx.x....xx....x..xx$*x*$xx..x....xx..xxxxxxxxx");
    strcpy(vgrid[22], "xxxxxxxxx..x.xxxxxxxxxxx99x........x.x....xx....x..x$$x|x$$x..x....xx..xxxxxxxxx");
    strcpy(vgrid[23], "xxxxxxxxx..x.x.........x88x.$$$$$$.x.x....xx..8.x..xx$*x*$xx..x....xx..xxxxxxxxx");
    strcpy(vgrid[24], "xxxxxxxxx..x.x.|xx|xx|.=..x.$$$$$$.x.x....xx....x...=x$$$xx...x..9.xx..xxxxxxxxx");
    strcpy(vgrid[25], "xxxxxxxxx..x.x..xx.xx..x..x.$$$$$$.x.x....xx....xxx9.xxxxx.8xxx....xx..xxxxxxxxx");
    strcpy(vgrid[26], "xxxxxxxxx..x.x.|..|..|.x..x.$$$$$$.x.x....xx......x.........x......xx..xxxxxxxxx");
    strcpy(vgrid[27], "xxxxxxxxx..x.x..xx.xx..x..x.$$$$$$.x.x....xx......xxxxxxxxxxx......xx..xxxxxxxxx");
    strcpy(vgrid[28], "xxxxxxxxx..x.x.|xx|xx|.x..x.$$$$$$.x.x....xxx.....................xxx..xxxxxxxxx");
    strcpy(vgrid[29], "xxxxxxxxx..x.x.........x..=........x.x....xxx=x....9.........9....xxx..xxxxxxxxx");
    strcpy(vgrid[30], "xxxxxxxxx..x.xxxxxxxxxxx..xxxxxxxxxx=xx.11....xx................xxxxx..xxxxxxxxx");
    strcpy(vgrid[31], "xxxxxxxxx..x9.........8=99x8.......=.xx1111...xxxxxxxxxxxxxxxxxxxxxxx..xxxxxxxxx");
    strcpy(vgrid[32], "xxxxxxxxx..xxxxxxxxxxxxxxxxxxxxxxxxxx=1....1..xxxxxxxxxxxxxxxxxxxxxxx..xxxxxxxxx");
    strcpy(vgrid[33], "xxxxxxxxx..........................xx1..(}..1..........................xxxxxxxxx");
    strcpy(vgrid[34], "xxxxxxxxx...........................11.[..{.11.........................xxxxxxxxx");
    strcpy(vgrid[35], "xxxxxxxxx............................1..])..1..........................xxxxxxxxx");
    strcpy(vgrid[36], "xxxxxxxxx.............................1....1...........................xxxxxxxxx");
    strcpy(vgrid[37], "xxxxxxxxx..xxxxxxxxxxxxxxxxxxxxxxx=....1111..x.xxx.xxxxxxxxxxxxxxxxxx..xxxxxxxxx");
    strcpy(vgrid[38], "xxxxxxxxx..xx.x.x.x.x.x.x.x.x.x.x.x=....11..........................x..xxxxxxxxx");
    strcpy(vgrid[39], "xxxxxxxxx..x.x.x.x.x.x.x.x|x.x.x.x.x=...............................x..xxxxxxxxx");
    strcpy(vgrid[40], "xxxxxxxxx..xx.x|x.x.x.x.x.x.x.x.x.x.x=....x.........................x..xxxxxxxxx");
    strcpy(vgrid[41], "xxxxxxxxx..x.x.x.x.x.x.x.x9x.x.x.x.x.x..........8..........9........x..xxxxxxxxx");
    strcpy(vgrid[42], "xxxxxxxxx..xx.x.x.x.x.x.x.x.x.x.x.x.xx....x..9......................x..xxxxxxxxx");
    strcpy(vgrid[43], "xxxxxxxxx..x.x.x.x.x.x.x.x.x.x|x.x.x.x....x.........................x..xxxxxxxxx");
    strcpy(vgrid[44], "xxxxxxxxx..xx.x8x.x.x|x.x.x.x.x.x.x.xx....x..............9...9......x..xxxxxxxxx");
    strcpy(vgrid[45], "xxxxxxxxx..x.x.x.x.x9x.x.x.x.x.x.x.x.x...........8..................x..xxxxxxxxx");
    strcpy(vgrid[46], "xxxxxxxxx..xx.x.x.x.x.x.x.x.x.x.x.x.xx....x..9......................x..xxxxxxxxx");
    strcpy(vgrid[47], "xxxxxxxxx..x.x.x.x.x.x.x.x.x|x.x9x.x.x....x.........................x..xxxxxxxxx");
    strcpy(vgrid[48], "xxxxxxxxx..xx.x.x.x.x.x.x.x.x.x.x.x.xx....x...................9.....x..xxxxxxxxx");
    strcpy(vgrid[49], "xxxxxxxxx..x.x.x9x.x.x.x.x.x.x.x.x.x.x....x....9......8.............x..xxxxxxxxx");
    strcpy(vgrid[50], "xxxxxxxxx..xx.x.x.x.x.x.x9x.x.x.x.x.xx....x.........................x..xxxxxxxxx");
    strcpy(vgrid[51], "xxxxxxxxx..x.x.x.x.x.x.x.x.x.x.x.x.x.x....x.........................x..xxxxxxxxx");
    strcpy(vgrid[52], "xxxxxxxxx..xx.x.x.x.x.x.x.x.x.x.x.x.xx....x.......9......9..........x..xxxxxxxxx");
    strcpy(vgrid[53], "xxxxxxxxx..x.x.x.x.x.x.x.x.x.x8x.x.x.x....x.....................8...x..xxxxxxxxx");
    strcpy(vgrid[54], "xxxxxxxxx..xx.x8x.x.x.x.x.x.x.x.x.x.xx....x.....................||.|x..xxxxxxxxx");
    strcpy(vgrid[55], "xxxxxxxxx..x.x|x.x.x.x.x.x.x|x.x.x.x.x....x.....................|...x..xxxxxxxxx");
    strcpy(vgrid[56], "xxxxxxxxx..xx.x.x.x.x.x.x8x.x.x.x.x.xx....x......8..................x..xxxxxxxxx");
    strcpy(vgrid[57], "xxxxxxxxx..x=x.x.x.x.x.x.x.x.x.x.x.x.x....x..........8..8...8.....||x..xxxxxxxxx");
    strcpy(vgrid[58], "xxxxxxxxx..xO=x.x.x.x.x.x.x.x.x.x|x.xx....x.....................|.||x..xxxxxxxxx");
    strcpy(vgrid[59], "xxxxxxxxx..xxxxxxxxxxxxxxxxxxxxxxxxxxx....xxxxxxxxxxxxxxxxxxxxxxxxxxx..xxxxxxxxx");
    strcpy(vgrid[60], "xxxxxxxxx..............................................................xxxxxxxxx");
    strcpy(vgrid[61], "xxxxxxxxx..............................................................xxxxxxxxx");
    strcpy(vgrid[62], "xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx");
    strcpy(vgrid[63], "xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx");
    strcpy(vgrid[64], "xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx");
    strcpy(vgrid[65], "xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx");
    strcpy(vgrid[66], "xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx");
    strcpy(vgrid[67], "xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx");
    strcpy(vgrid[68], "xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx");
    strcpy(vgrid[69], "xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx");

    mons_array[0] = MONS_VAULT_GUARD;
    mons_array[1] = RANDOM_MONSTER;
    mons_array[2] = RANDOM_MONSTER;
    mons_array[3] = RANDOM_MONSTER;
    mons_array[4] = RANDOM_MONSTER;
    mons_array[5] = RANDOM_MONSTER;
    mons_array[6] = RANDOM_MONSTER;

    return MAP_ENCOMPASS;
}


static char snake_pit(char vgrid[81][81], FixedVector<int, 7>& mons_array)
{     // Hey, this looks a bit like a face ...

    for (unsigned char i = 0; i < 81; i++)
        strcpy(vgrid[i], "xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx");

    strcpy(vgrid[36], "xxxxxxxxxxxxxxxxxxxxxxx..@.xxxxxxxxxxxxx");
    strcpy(vgrid[37], "xxxxxxxxxxxxxxxxxxx.............xxxxxxxx");
    strcpy(vgrid[38], "xxxxxxxxxxxxxx....x.............x..xxxxx");
    strcpy(vgrid[39], "xxxxxxxxxxxx....2.x.............x.2..xxx");
    strcpy(vgrid[40], "xxxxxxxxxxx.....2.x....x.....x..x..3.xxx");
    strcpy(vgrid[41], "xxxxxxxxxxx.....22x.............xx.2..xx");
    strcpy(vgrid[42], "xxxxxxxxxxx.......xx..x........xx..3..xx");
    strcpy(vgrid[43], "xxxxxxxxxx.....x23.xx....T...xxx.44...xx");
    strcpy(vgrid[44], "xxxxxxxxxx......4.4.x.........x.333....x");
    strcpy(vgrid[45], "xxxxxxxxxx......3.x4...x.......4x4.....x");
    strcpy(vgrid[46], "xxxxxxxxxx.......3.......x.............x");
    strcpy(vgrid[47], "xxxxxxxxxx..c......3.........x.......c.x");
    strcpy(vgrid[48], "xxxxxxxxx...cc...................3..cc.x");
    strcpy(vgrid[49], "xxxxxxxxx...cc..........4.4.........cc.x");
    strcpy(vgrid[50], "xxxxxxxxx...cc...3...x........2.....cc.x");
    strcpy(vgrid[51], "xxxxxxxxx...cc.........1...1.......cc..x");
    strcpy(vgrid[52], "xxxxxxxxxx..cc.....1.....1.....1..ccc.xx");
    strcpy(vgrid[53], "xxxxxxxxxx...ccc..................cc..xx");
    strcpy(vgrid[54], "xxxxxxxxxx....cccc....3333333.....cc..xx");
    strcpy(vgrid[55], "xxxxxxxxxx.....ccccccc...........cc...xx");
    strcpy(vgrid[56], "xxxxxxxxxx........cccccccO...ccccc....xx");
    strcpy(vgrid[57], "xxxxxxxxxxx...4....cccccccccccccc....xxx");
    strcpy(vgrid[58], "xxxxxxxxxxx.........cccccccccccc..1..xxx");
    strcpy(vgrid[59], "xxxxxxxxxxxxx.......................xxxx");
    strcpy(vgrid[60], "xxxxxxxxxxxxxxxx........3.........xxxxxx");
    strcpy(vgrid[61], "xxxxxxxxxxxxxxxxxxxxx.......xxxxxxxxxxxx");
    strcpy(vgrid[62], "xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx");
    strcpy(vgrid[63], "xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx");
    strcpy(vgrid[64], "xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx");
    strcpy(vgrid[65], "xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx");
    strcpy(vgrid[66], "xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx");
    strcpy(vgrid[67], "xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx");
    strcpy(vgrid[68], "xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx");
    strcpy(vgrid[69], "xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx");

    mons_array[0] = MONS_GREATER_NAGA;
    mons_array[1] = MONS_NAGA;
    mons_array[2] = MONS_NAGA_MAGE;
    mons_array[3] = MONS_NAGA_WARRIOR;
    mons_array[4] = RANDOM_MONSTER;
    mons_array[5] = RANDOM_MONSTER;
    mons_array[6] = RANDOM_MONSTER;

    return MAP_SOUTHWEST;
}


static char elf_hall(char vgrid[81][81], FixedVector<int, 7>& mons_array)
{

    strcpy(vgrid[0],  "xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx");
    strcpy(vgrid[1],  "xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx");
    strcpy(vgrid[2],  "xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx");
    strcpy(vgrid[3],  "xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx");
    strcpy(vgrid[4],  "xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx");
    strcpy(vgrid[5],  "xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx");
    strcpy(vgrid[6],  "xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx");
    strcpy(vgrid[7],  "xxxxxxxxxxxxxxxxxxcccccccccccccccccxxxxx");
    strcpy(vgrid[8],  "xxxxxxxxxxxxxxxxxcc%%%%%%~~~~~~~c$ccxxxx");
    strcpy(vgrid[9],  "xxxxxxxxxxxxxxxxcc%%%%%%~~~~~~~~c$$ccxxx");
    strcpy(vgrid[10], "xxxxxxxxxxxxxxxcc%%%%%%%%~~~~~~~c$$$ccxx");
    strcpy(vgrid[11], "xxxxxxxxxxxxxxcc%%%%%%ccccccccccc$$$$ccx");
    strcpy(vgrid[12], "xxxxxxxxxxxxxxc%%%%%%ccc.....2..cB$$$$cx");
    strcpy(vgrid[13], "xxxxxxxxxxxxxxc%%%%cAc...........B$$$$cx");
    strcpy(vgrid[14], "xxxxxxxxxxxxxxc%%%%Ac...5.......4Bc$$$cx");
    strcpy(vgrid[15], "xxxxxxxxxxxxxxc%%%cA........3..cccBBBBcx");
    strcpy(vgrid[16], "xxxxxxxxxxxxxxc%AAAc.....2....cc.....5cx");
    strcpy(vgrid[17], "xxxxxxxxxxxxxxc%A....5.......cc.......cx");
    strcpy(vgrid[18], "xxxxxxxxxxxxxxccA......5....cc..2....ccx");
    strcpy(vgrid[19], "xxxxxxxxxxxxxxxxc..........cc.......ccxx");
    strcpy(vgrid[20], "xxxxxxxxxxxxxxxcc..1..U..........4..ccxx");
    strcpy(vgrid[21], "xxxxxxxxxxxxxxcc.....................ccx");
    strcpy(vgrid[22], "xxxxxxxxxxxxxxc...........3...........cx");
    strcpy(vgrid[23], "xxxxxxxxxxxxxxc.......2.......3.......cx");
    strcpy(vgrid[24], "xxxxxxxxxxxxxxc..2................2..5cx");
    strcpy(vgrid[25], "xxxxxxxxxxxxxxc......x.........x......cx");
    strcpy(vgrid[26], "xxxxxxxxxxxxxxc.....xx.........xx.....cx");
    strcpy(vgrid[27], "xxxxxxxxxxxxxxc2...xxx....1....xxx.4..cx");
    strcpy(vgrid[28], "xxxxxxxxxxxxxxc..xxxx...........xxxx..cx");
    strcpy(vgrid[29], "xxxxxxxxxxxxxxc.xxx.....cc=cc.....xxx.cx");
    strcpy(vgrid[30], "xxxxxxxxxxxxxxc.x.....cccc.cccc.....x.cx");
    strcpy(vgrid[31], "xxxxxxxxxxxxxxc.3...cccxxc.cxxccc.3...cx");
    strcpy(vgrid[32], "xxxxxxxxxxxxxxc...cccxxxxc.cxxxxccc...cx");
    strcpy(vgrid[33], "xxxxxxxxxxxxxxc.cccxxxxxxc.cxxxxxxccc.cx");
    strcpy(vgrid[34], "xxxxxxxxxxxxxxcccxxxxxxxxc.cxxxxxxxxcccx");
    strcpy(vgrid[35], "xxxxxxxxxxxxxxxxxxxxxxxxxx@xxxxxxxxxxxxx");

    mons_array[0] = MONS_DEEP_ELF_HIGH_PRIEST;
    mons_array[1] = MONS_DEEP_ELF_DEMONOLOGIST;
    mons_array[2] = MONS_DEEP_ELF_ANNIHILATOR;
    mons_array[3] = MONS_DEEP_ELF_SORCERER;
    mons_array[4] = MONS_DEEP_ELF_DEATH_MAGE;
    mons_array[5] = RANDOM_MONSTER;
    mons_array[6] = RANDOM_MONSTER;

    replace_one_of( vgrid, 40, 20, 'A', '=', 'c' );
    replace_one_of( vgrid, 40, 20, 'B', '=', 'c' );

    replace_with_func( vgrid, 40, 20, '%', trapped_treasure_area );
    replace_with_func( vgrid, 40, 20, '~', great_treasure_area );

    return MAP_NORTHWEST;
}


// Slime pit take is reduced pending an increase in difficulty
// of this subdungeon. -- bwr
static char slime_pit(char vgrid[81][81], FixedVector<int, 7>& mons_array)
{

    strcpy(vgrid[0], "xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx");
    strcpy(vgrid[1], "xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx");
    strcpy(vgrid[2], "xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx");
    strcpy(vgrid[3], "xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx");
    strcpy(vgrid[4], "xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx");
    strcpy(vgrid[5], "xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx");
    strcpy(vgrid[6], "xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx");
    strcpy(vgrid[7], "xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx....xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx");
    strcpy(vgrid[8], "xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx..xxxx.........xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx");
    strcpy(vgrid[9], "xxxxxxxxxxxxxxxxxxxxxxxxxxxxx....................xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx");
    strcpy(vgrid[10], "xxxxxxxxxxxxxxxxxxxxxxxxxxxx......................xxxxxxxxxxxxxxxxxxxxxxxxxxxxxx");
    strcpy(vgrid[11], "xxxxxxxxxxxxxxxxxxxxxxxxxx..........................x.xxxxxxxxxxxxxxxxxxxxxxxxxx");
    strcpy(vgrid[12], "xxxxxxxxxxxxxxxxxxxxxxxxxx............................xxxxxxxxxxxxxxxxxxxxxxxxxx");
    strcpy(vgrid[13], "xxxxxxxxxxxxxxxxxxxxxxxxx.............................xxxxxxxxxxxxxxxxxxxxxxxxxx");
    strcpy(vgrid[14], "xxxxxxxxxxxxxxxxxxxxxxxx.................................xxxxxxxxxxxxxxxxxxxxxxx");
    strcpy(vgrid[15], "xxxxxxxxxxxxxxxxxxxxxxxx..................................xxxxxxxxxxxxxxxxxxxxxx");
    strcpy(vgrid[16], "xxxxxxxxxxxxxxxxxxxxxx....(................................xxxxxxxxxxxxxxxxxxxxx");
    strcpy(vgrid[17], "xxxxxxxxxxxxxxxxxxxxxx......................................xxxxxxxxxxxxxxxxxxxx");
    strcpy(vgrid[18], "xxxxxxxxxxxxxxxxxxx..........................................xxxxxxxxxxxxxxxxxxx");
    strcpy(vgrid[19], "xxxxxxxxxxxxxxxxxxx..........................................xxxxxxxxxxxxxxxxxxx");
    strcpy(vgrid[20], "xxxxxxxxxxxxxxxxx............................................xxxxxxxxxxxxxxxxxxx");
    strcpy(vgrid[21], "xxxxxxxxxxxxxxxxx............................................xxxxxxxxxxxxxxxxxxx");
    strcpy(vgrid[22], "xxxxxxxxxxxxxx.....................ccc2.ccc............]......xxxxxxxxxxxxxxxxxx");
    strcpy(vgrid[23], "xxxxxxxxxxxxxxx...................cccc=ccccc...................xxxxxxxxxxxxxxxxx");
    strcpy(vgrid[24], "xxxxxxxxxxxxxx...................cc~cc..cc~cc....................xxxxxxxxxxxxxxx");
    strcpy(vgrid[25], "xxxxxxxxxxxxxx..................cc~~~cc4c~~~cc..................xxxxxxxxxxxxxxxx");
    strcpy(vgrid[26], "xxxxxxxxxxxxx..................cc~~~cc..cc~~~cc..................xxxxxxxxxxxxxxx");
    strcpy(vgrid[27], "xxxxxxxxxxxxx.................cc~~O~~c4cc~~O~~cc.................xxxxxxxxxxxxxxx");
    strcpy(vgrid[28], "xxxxxxxxxxxxx.................cc~~~~cc..cc~~~~cc....................xxxxxxxxxxxx");
    strcpy(vgrid[29], "xxxxxxxxxxxx..................ccc~~c~cc4c~c~~ccc...................xxxxxxxxxxxxx");
    strcpy(vgrid[30], "xxxxxxxxxxxx..................cccccccc..cccccccc....................xxxxxxxxxxxx");
    strcpy(vgrid[31], "xxxxxxxxxxx...................c.4.c.4.1..4.c.4.c.....................xxxxxxxxxxx");
    strcpy(vgrid[32], "xxxxxxxxxxx...................=2c.4.c..3.c.4.c2=.....................xxxxxxxxxxx");
    strcpy(vgrid[33], "xxxxxxxxxxx..........)........cccccccc..cccccccc.....................xxxxxxxxxxx");
    strcpy(vgrid[34], "xxxxxxxxxxx...................ccc~~c~cc4c~c~~ccc.....................xxxxxxxxxxx");
    strcpy(vgrid[35], "xxxxxxxxxx....................cc~~~~cc..cc~~~~cc....................xxxxxxxxxxxx");
    strcpy(vgrid[36], "xxxxxxxxxx....................cc~~O~~c4cc~~O~~cc....................xxxxxxxxxxxx");
    strcpy(vgrid[37], "xxxxxxxxxx.....................cc~~~cc..cc~~~cc....................xxxxxxxxxxxxx");
    strcpy(vgrid[38], "xxxxxxxxxxx.....................cc~~~cc4c~~~cc.....................xxxxxxxxxxxxx");
    strcpy(vgrid[39], "xxxxxxxxxxxx.....................cc~cc..cc~cc......................xxxxxxxxxxxxx");
    strcpy(vgrid[40], "xxxxxxxxxxxxx.....................cccc=ccccc......................xxxxxxxxxxxxxx");
    strcpy(vgrid[41], "xxxxxxxxxxxxxx.....................ccc2.ccc.......................xxxxxxxxxxxxxx");
    strcpy(vgrid[42], "xxxxxxxxxxxxxx...........................................[.........xxxxxxxxxxxxx");
    strcpy(vgrid[43], "xxxxxxxxxxxxx......................................................xxxxxxxxxxxxx");
    strcpy(vgrid[44], "xxxxxxxxxxxxx..............................................xxxxx...xxxxxxxxxxxxx");
    strcpy(vgrid[45], "xxxxxxxxxxxxxx...........................................xxxxxxxx.xxxxxxxxxxxxxx");
    strcpy(vgrid[46], "xxxxxxxxxxxxxx..........................................xxxxxxxxx.xxxxxxxxxxxxxx");
    strcpy(vgrid[47], "xxxxxxxxxxxxxxxx........................................xxxxxxxxxxxxxxxxxxxxxxxx");
    strcpy(vgrid[48], "xxxxxxxxxxxxxxxx.........................................xxxxxxxxxxxxxxxxxxxxxxx");
    strcpy(vgrid[49], "xxxxxxxxxxxxxxxxxx.......................................xxxxxxxxxxxxxxxxxxxxxxx");
    strcpy(vgrid[50], "xxxxxxxxxxxxxxxxxxxx......................................xxxxxxxxxxxxxxxxxxxxxx");
    strcpy(vgrid[51], "xxxxxxxxxxxxxxxxxxxx......................................xxxxxxxxxxxxxxxxxxxxxx");
    strcpy(vgrid[52], "xxxxxxxxxxxxxxxxxxxxx.....................................xxxxxxxxxxxxxxxxxxxxxx");
    strcpy(vgrid[53], "xxxxxxxxxxxxxxxxxxxxx.............................}......xxxxxxxxxxxxxxxxxxxxxxx");
    strcpy(vgrid[54], "xxxxxxxxxxxxxxxxxxxxxxx.................................xxxxxxxxxxxxxxxxxxxxxxxx");
    strcpy(vgrid[55], "xxxxxxxxxxxxxxxxxxxxxxxx..............................xxxxxxxxxxxxxxxxxxxxxxxxxx");
    strcpy(vgrid[56], "xxxxxxxxxxxxxxxxxxxxxxxxx..............................xxxxxxxxxxxxxxxxxxxxxxxxx");
    strcpy(vgrid[57], "xxxxxxxxxxxxxxxxxxxxxxxxxx............................xxxxxxxxxxxxxxxxxxxxxxxxxx");
    strcpy(vgrid[58], "xxxxxxxxxxxxxxxxxxxxxxxxxxxx...........{........xxx..xxxxxxxxxxxxxxxxxxxxxxxxxxx");
    strcpy(vgrid[59], "xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx................xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx");
    strcpy(vgrid[60], "xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx.........xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx");
    strcpy(vgrid[61], "xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx...xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx");
    strcpy(vgrid[62], "xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx");
    strcpy(vgrid[63], "xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx");
    strcpy(vgrid[64], "xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx");
    strcpy(vgrid[65], "xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx");
    strcpy(vgrid[66], "xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx");
    strcpy(vgrid[67], "xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx");
    strcpy(vgrid[68], "xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx");
    strcpy(vgrid[69], "xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx");

    mons_array[0] = MONS_ROYAL_JELLY;
    mons_array[1] = MONS_ACID_BLOB;
    mons_array[2] = MONS_GREAT_ORB_OF_EYES;
    mons_array[3] = RANDOM_MONSTER;
    mons_array[4] = RANDOM_MONSTER;
    mons_array[5] = RANDOM_MONSTER;
    mons_array[6] = RANDOM_MONSTER;

    replace_one_of( vgrid, 50, 42, '=', '=', 'c' );
    replace_one_of( vgrid, 50, 42, 'O', 'O', '|' );

    replace_with_func( vgrid, 50, 42, '~', great_treasure_area );

    return MAP_ENCOMPASS;

}


static char hall_of_blades(char vgrid[81][81], FixedVector<int, 7>& mons_array)
{

    strcpy(vgrid[0], "xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx");
    strcpy(vgrid[1], "xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx");
    strcpy(vgrid[2], "xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx");
    strcpy(vgrid[3], "xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx");
    strcpy(vgrid[4], "xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx");
    strcpy(vgrid[5], "xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx");
    strcpy(vgrid[6], "xxxxxxxxccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccxxxxxxxx");
    strcpy(vgrid[7], "xxxxxxxxccc....cccc.cccc.cccc.cccc.cccc.cccc.cccc.cccc.cccc.cccc.....cccxxxxxxxx");
    strcpy(vgrid[8], "xxxxxxxxcc......cc...cc...cc...cc...cc...cc...cc...cc...cc...cc.......ccxxxxxxxx");
    strcpy(vgrid[9], "xxxxxxxxc..............................................................cxxxxxxxx");
    strcpy(vgrid[10], "xxxxxxxxc..........c..............c..............c..............c......cxxxxxxxx");
    strcpy(vgrid[11], "xxxxxxxxc.........ccc............ccc............ccc............ccc.....cxxxxxxxx");
    strcpy(vgrid[12], "xxxxxxxxc........ccccc..........ccccc..........ccccc..........ccccc....cxxxxxxxx");
    strcpy(vgrid[13], "xxxxxxxxc.........ccc............ccc............ccc...........ccccc....cxxxxxxxx");
    strcpy(vgrid[14], "xxxxxxxxc..........c..............c..............c.............ccc.....cxxxxxxxx");
    strcpy(vgrid[15], "xxxxxxxxc......................................................ccc.....cxxxxxxxx");
    strcpy(vgrid[16], "xxxxxxxxc.......................................................c......cxxxxxxxx");
    strcpy(vgrid[17], "xxxxxxxxc..............................................................cxxxxxxxx");
    strcpy(vgrid[18], "xxxxxxxxc..............................................................cxxxxxxxx");
    strcpy(vgrid[19], "xxxxxxxxc..............................................................cxxxxxxxx");
    strcpy(vgrid[20], "xxxxxxxxc..............................................................cxxxxxxxx");
    strcpy(vgrid[21], "xxxxxxxxc..............................................................cxxxxxxxx");
    strcpy(vgrid[22], "xxxxxxxxc..............................................................cxxxxxxxx");
    strcpy(vgrid[23], "xxxxxxxxc.......................................................c......cxxxxxxxx");
    strcpy(vgrid[24], "xxxxxxxxc......................................................ccc.....cxxxxxxxx");
    strcpy(vgrid[25], "xxxxxxxxc..........c..............c..............c.............ccc.....cxxxxxxxx");
    strcpy(vgrid[26], "xxxxxxxxc.........ccc............ccc............ccc...........ccccc....cxxxxxxxx");
    strcpy(vgrid[27], "xxxxxxxxc........ccccc..........ccccc..........ccccc..........ccccc....cxxxxxxxx");
    strcpy(vgrid[28], "xxxxxxxxc.........ccc............ccc............ccc............ccc.....cxxxxxxxx");
    strcpy(vgrid[29], "xxxxxxxxc..........c..............c..............c..............c......cxxxxxxxx");
    strcpy(vgrid[30], "xxxxxxxxc..............................................................cxxxxxxxx");
    strcpy(vgrid[31], "xxxxxxxxc.......cc...cc...cc...cc...cc...cc...cc...cc...cc...cc.......ccxxxxxxxx");
    strcpy(vgrid[32], "xxxxxxxxcc.....cccc.cccc.cccc.cccc.cccc.cccc.cccc.cccc.cccc.cccc.....cccxxxxxxxx");
    strcpy(vgrid[33], "xxxxxxxxccc...ccccccccccccccccccccccccccccccccccccccccccccccccccccccccccxxxxxxxx");
    strcpy(vgrid[34], "xxxxxxxxcccc.............................cccccccccccccccccccccccccccccccxxxxxxxx");
    strcpy(vgrid[35], "xxxxxxxxcccccccccccccccccccccccccccccc.@.cccccccccccccccccccccccccccccccxxxxxxxx");

    mons_array[0] = MONS_DANCING_WEAPON;
    mons_array[1] = RANDOM_MONSTER;
    mons_array[2] = RANDOM_MONSTER;
    mons_array[3] = RANDOM_MONSTER;
    mons_array[4] = RANDOM_MONSTER;
    mons_array[5] = RANDOM_MONSTER;
    mons_array[6] = RANDOM_MONSTER;

    return MAP_NORTH;
}


static char hall_of_Zot(char vgrid[81][81], FixedVector<int, 7>& mons_array)
{

    strcpy(vgrid[0],  "xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx");
    strcpy(vgrid[1],  "xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx");
    strcpy(vgrid[2],  "xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx");
    strcpy(vgrid[3],  "xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx");
    strcpy(vgrid[4],  "xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx");
    strcpy(vgrid[5],  "xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx");
    strcpy(vgrid[6],  "xxxxxxxxxxxxxxccccccccccccccccxxxxxxxxxxxxxxxxxxxxxxxccccccccccccccxxxxxxxxxxxxx");
    strcpy(vgrid[7],  "xxxxxxxxxxxcccc..............ccccxxxxxxxxxxxxxxxxxcccc............ccccxxxxxxxxxx");
    strcpy(vgrid[8],  "xxxxxxxxxxcc....................cccxxxxxxxxxxxxxccc..................ccxxxxxxxxx");
    strcpy(vgrid[9],  "xxxxxxxxxcc...........8...........XXXXxxxxxxxXXXX...........8.........ccxxxxxxxx");
    strcpy(vgrid[10], "xxxxxxxxxc..8......................XXXXXXXXXXXXX....................8..cxxxxxxxx");
    strcpy(vgrid[11], "xxxxxxxxxc...................8.....XXX...1...XXX....8..................cxxxxxxxx");
    strcpy(vgrid[12], "xxxxxxxxxcc......................^^XX..1...1..XX^^....................ccxxxxxxxx");
    strcpy(vgrid[13], "xxxxxxxxxxcc.....................^^X1.........1X^^...................ccxxxxxxxxx");
    strcpy(vgrid[14], "xxxxxxxxxxxcc.....5....2.........^^&..1..Z..1..&^^.......2....5.....ccxxxxxxxxxx");
    strcpy(vgrid[15], "xxxxxxxxxxcc.....................^^X1.........1X^^...................ccxxxxxxxxx");
    strcpy(vgrid[16], "xxxxxxxxxcc......................^^XX..1...1..XX^^....................ccxxxxxxxx");
    strcpy(vgrid[17], "xxxxxxxxxc...................8.....XXX...1...XXX....8..................cxxxxxxxx");
    strcpy(vgrid[18], "xxxxxxxxxc...8.....................XXXXXXXXXXXXX...................8...cxxxxxxxx");
    strcpy(vgrid[19], "xxxxxxxxxcc..........8............XXXXcccccccXXXX..........8..........ccxxxxxxxx");
    strcpy(vgrid[20], "xxxxxxxxxxcc....................ccccccccccccccccccc..................ccxxxxxxxxx");
    strcpy(vgrid[21], "xxxxxxxxxxxcc.^^^^^..........ccccccccccccccccccccccccc........^^^^^.ccxxxxxxxxxx");
    strcpy(vgrid[22], "xxxxxxxxxxxxccF^^^FcccccccccccccccccccccccccccccccccccccccccccF^^^Fccxxxxxxxxxxx");
    strcpy(vgrid[23], "xxxxxxxxxxxcc..111...........^^^^ccccccccccccccccc^^^^.........111..ccxxxxxxxxxx");
    strcpy(vgrid[24], "xxxxxxxxxxcc.................cc^^^^ccccccccccccc^^^^cc...............ccxxxxxxxxx");
    strcpy(vgrid[25], "xxxxxxxxxcc.............8.....ccc^^1ccccccccccc1^^ccc...8.............ccxxxxxxxx");
    strcpy(vgrid[26], "xxxxxxxxxc....8................ccc1.............1ccc...........8...8...cxxxxxxxx");
    strcpy(vgrid[27], "xxxxxxxxxc.......8.....8...8...cxcc.............ccxc...................cxxxxxxxx");
    strcpy(vgrid[28], "xxxxxxxxxc.....................cxxc.............cxxc.......8...........cxxxxxxxx");
    strcpy(vgrid[29], "xxxxxxxxxc.8...................cxxcc.1...1...1.ccxxc............8....8.cxxxxxxxx");
    strcpy(vgrid[30], "xxxxxxxxxc.......8....8.....8..cxxxc...........cxxxc....8..............cxxxxxxxx");
    strcpy(vgrid[31], "xxxxxxxxxc.....................cxxcc...........ccxxc..........8........cxxxxxxxx");
    strcpy(vgrid[32], "xxxxxxxxxcc...4...............ccxxc.............cxxcc..............8..ccxxxxxxxx");
    strcpy(vgrid[33], "xxxxxxxxxxcc........8........ccxxcc....^^^^^....ccxxcc....8....3.....ccxxxxxxxxx");
    strcpy(vgrid[34], "xxxxxxxxxxxcc...............ccxxxc.....^^^^^.....cxxxcc.............ccxxxxxxxxxx");
    strcpy(vgrid[35], "xxxxxxxxxxxxcccccccccccccccccxxxxcccccccc@ccccccccxxxxcccccccccccccccxxxxxxxxxxx");

    mons_array[0] = MONS_ORB_GUARDIAN;
    mons_array[1] = MONS_KILLER_KLOWN;
    mons_array[2] = MONS_ELECTRIC_GOLEM;
    mons_array[3] = MONS_ORB_OF_FIRE;
    mons_array[4] = MONS_ANCIENT_LICH;
    mons_array[5] = RANDOM_MONSTER;
    mons_array[6] = RANDOM_MONSTER;

    replace_one_of( vgrid, 70, 35, 'F', (coinflip() ? 'S' : 'H'), 'G' );
    replace_with_func( vgrid, 70, 35, '^', trapped_area );

    return MAP_NORTH;
}


static char temple(char vgrid[81][81], FixedVector<int, 7>& mons_array)
{     // this is the ecumenical temple level
    UNUSED( mons_array );

    strcpy(vgrid[0],  "xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx");
    strcpy(vgrid[1],  "xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx");
    strcpy(vgrid[2],  "xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx");
    strcpy(vgrid[3],  "xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx");
    strcpy(vgrid[4],  "xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx");
    strcpy(vgrid[5],  "xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx");
    strcpy(vgrid[6],  "xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx");
    strcpy(vgrid[7],  "xxxxxxxxxxxxxxxxxxxxxxxxxxccccccccccccccccccccccccccxxxxxxxxxxxxxxxxxxxxxxxxxxxx");
    strcpy(vgrid[8],  "xxxxxxxxxxxxxxxxxxxxxxxxxcc............<............cxxxxxxxxxxxxxxxxxxxxxxxxxxx");
    strcpy(vgrid[9],  "xxxxxxxxxxxxxxxxxxxxxxxxcc...........................cxxxxxxxxxxxxxxxxxxxxxxxxxx");
    strcpy(vgrid[10], "xxxxxxxxxxxxxxxxxxxxxxxcc.............................cxxxxxxxxxxxxxxxxxxxxxxxxx");
    strcpy(vgrid[11], "xxxxxxxxxxxxxxxxxxxxxxcc...............................cxxxxxxxxxxxxxxxxxxxxxxxx");
    strcpy(vgrid[12], "xxxxxxxxxxxxxxxxxxxxxcc.................................cxxxxxxxxxxxxxxxxxxxxxxx");
    strcpy(vgrid[13], "xxxxxxxxxxxxxxxxxxxxcc...................................cxxxxxxxxxxxxxxxxxxxxxx");
    strcpy(vgrid[14], "xxxxxxxxxxxxxxxxxxxcc.....................................cxxxxxxxxxxxxxxxxxxxxx");
    strcpy(vgrid[15], "xxxxxxxxxxxxxxxxxxcc.......................................cxxxxxxxxxxxxxxxxxxxx");
    strcpy(vgrid[16], "xxxxxxxxxxxxxxxxxcc.........................................cxxxxxxxxxxxxxxxxxxx");
    strcpy(vgrid[17], "xxxxxxxxxxxxxxxxcc...........................................cxxxxxxxxxxxxxxxxxx");
    strcpy(vgrid[18], "xxxxxxxxxxxxxxxcc.............................................cxxxxxxxxxxxxxxxxx");
    strcpy(vgrid[19], "xxxxxxxxxxxxxxcc...............................................cxxxxxxxxxxxxxxxx");
    strcpy(vgrid[20], "xxxxxxxxxxxxxcc.................................................cxxxxxxxxxxxxxxx");
    strcpy(vgrid[21], "xxxxxxxxxxxxcc...................................................cxxxxxxxxxxxxxx");
    strcpy(vgrid[22], "xxxxxxxxxxxcc..........................B..........................cxxxxxxxxxxxxx");
    strcpy(vgrid[23], "xxxxxxxxxxcc.......................................................cxxxxxxxxxxxx");
    strcpy(vgrid[24], "xxxxxxxxxcc.....................B.............B.....................cxxxxxxxxxxx");
    strcpy(vgrid[25], "xxxxxxxxcc...........................................................cxxxxxxxxxx");
    strcpy(vgrid[26], "xxxxxxxxc.............................................................cxxxxxxxxx");
    strcpy(vgrid[27], "xxxxxxxxc.............................................................cxxxxxxxxx");
    strcpy(vgrid[28], "xxxxxxxxc.................B.........................B.................cxxxxxxxxx");
    strcpy(vgrid[29], "xxxxxxxxc..............................T..............................cxxxxxxxxx");
    strcpy(vgrid[30], "xxxxxxxxc.............................................................cxxxxxxxxx");
    strcpy(vgrid[31], "xxxxxxxxc.............................................................cxxxxxxxxx");
    strcpy(vgrid[32], "xxxxxxxxc.............................................................cxxxxxxxxx");
    strcpy(vgrid[33], "xxxxxxxxc.............................................................cxxxxxxxxx");
    strcpy(vgrid[34], "xxxxxxxxc.............................................................cxxxxxxxxx");
    strcpy(vgrid[35], "xxxxxxxxc(....................T.................T....................{cxxxxxxxxx");
    strcpy(vgrid[36], "xxxxxxxxc..............B...............................B..............cxxxxxxxxx");
    strcpy(vgrid[37], "xxxxxxxxc.............................................................cxxxxxxxxx");
    strcpy(vgrid[38], "xxxxxxxxc.............................................................cxxxxxxxxx");
    strcpy(vgrid[39], "xxxxxxxxc.............................................................cxxxxxxxxx");
    strcpy(vgrid[40], "xxxxxxxxc.............................................................cxxxxxxxxx");
    strcpy(vgrid[41], "xxxxxxxxc.............................................................cxxxxxxxxx");
    strcpy(vgrid[42], "xxxxxxxxc.............................................................cxxxxxxxxx");
    strcpy(vgrid[43], "xxxxxxxxc................B...........................B................cxxxxxxxxx");
    strcpy(vgrid[44], "xxxxxxxxcc...........................................................ccxxxxxxxxx");
    strcpy(vgrid[45], "xxxxxxxxxcc............................T............................ccxxxxxxxxxx");
    strcpy(vgrid[46], "xxxxxxxxxxcc.......................................................ccxxxxxxxxxxx");
    strcpy(vgrid[47], "xxxxxxxxxxxcc.....................................................ccxxxxxxxxxxxx");
    strcpy(vgrid[48], "xxxxxxxxxxxxcc...................................................ccxxxxxxxxxxxxx");
    strcpy(vgrid[49], "xxxxxxxxxxxxxcc.................................................ccxxxxxxxxxxxxxx");
    strcpy(vgrid[50], "xxxxxxxxxxxxxxcc...............B................B..............ccxxxxxxxxxxxxxxx");
    strcpy(vgrid[51], "xxxxxxxxxxxxxxxcc.............................................ccxxxxxxxxxxxxxxxx");
    strcpy(vgrid[52], "xxxxxxxxxxxxxxxxcc.....................B.....................ccxxxxxxxxxxxxxxxxx");
    strcpy(vgrid[53], "xxxxxxxxxxxxxxxxxcc.........................................ccxxxxxxxxxxxxxxxxxx");
    strcpy(vgrid[54], "xxxxxxxxxxxxxxxxxxcc.......................................ccxxxxxxxxxxxxxxxxxxx");
    strcpy(vgrid[55], "xxxxxxxxxxxxxxxxxxxcc.....................................ccxxxxxxxxxxxxxxxxxxxx");
    strcpy(vgrid[56], "xxxxxxxxxxxxxxxxxxxxcc...................................ccxxxxxxxxxxxxxxxxxxxxx");
    strcpy(vgrid[57], "xxxxxxxxxxxxxxxxxxxxxcc.................................ccxxxxxxxxxxxxxxxxxxxxxx");
    strcpy(vgrid[58], "xxxxxxxxxxxxxxxxxxxxxxcc...............................ccxxxxxxxxxxxxxxxxxxxxxxx");
    strcpy(vgrid[59], "xxxxxxxxxxxxxxxxxxxxxxxcc.............................ccxxxxxxxxxxxxxxxxxxxxxxxx");
    strcpy(vgrid[60], "xxxxxxxxxxxxxxxxxxxxxxxxcc...........................ccxxxxxxxxxxxxxxxxxxxxxxxxx");
    strcpy(vgrid[61], "xxxxxxxxxxxxxxxxxxxxxxxxxcc............[............ccxxxxxxxxxxxxxxxxxxxxxxxxxx");
    strcpy(vgrid[62], "xxxxxxxxxxxxxxxxxxxxxxxxxxcccccccccccccccccccccccccccxxxxxxxxxxxxxxxxxxxxxxxxxxx");
    strcpy(vgrid[63], "xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx");
    strcpy(vgrid[64], "xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx");
    strcpy(vgrid[65], "xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx");
    strcpy(vgrid[66], "xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx");
    strcpy(vgrid[67], "xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx");
    strcpy(vgrid[68], "xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx");
    strcpy(vgrid[69], "xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx");

    return MAP_ENCOMPASS;
}


static char tomb_1(char vgrid[81][81], FixedVector<int, 7>& mons_array)
{

    strcpy(vgrid[0], "xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx");
    strcpy(vgrid[1], "xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx");
    strcpy(vgrid[2], "xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx");
    strcpy(vgrid[3], "xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx");
    strcpy(vgrid[4], "xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx");
    strcpy(vgrid[5], "xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx");
    strcpy(vgrid[6], "xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx");
    strcpy(vgrid[7], "xxxxxxxxx(.............................[..............................{xxxxxxxxx");
    strcpy(vgrid[8], "xxxxxxxxx..............................................................xxxxxxxxx");
    strcpy(vgrid[9], "xxxxxxxxx..............................................................xxxxxxxxx");
    strcpy(vgrid[10], "xxxxxxxxx..............................................................xxxxxxxxx");
    strcpy(vgrid[11], "xxxxxxxxx..............................................................xxxxxxxxx");
    strcpy(vgrid[12], "xxxxxxxxx..........ccccccccccccccccccccccccccccccccccccccccccc.........xxxxxxxxx");
    strcpy(vgrid[13], "xxxxxxxxx..........ccccccccccccccccccccccccccccccccccccccccccc.........xxxxxxxxx");
    strcpy(vgrid[14], "xxxxxxxxx..........cc.........^...............^.....^.......cc.........xxxxxxxxx");
    strcpy(vgrid[15], "xxxxxxxxx..........cc.........^...............^.....^.......cc.........xxxxxxxxx");
    strcpy(vgrid[16], "xxxxxxxxx..........cc..ccccccccccccccccccccccccccccccccccc..cc.........xxxxxxxxx");
    strcpy(vgrid[17], "xxxxxxxxx..........cc..c....^^^^^^..c................c^^)c..cc.........xxxxxxxxx");
    strcpy(vgrid[18], "xxxxxxxxx..........cc..c..ccccccccc.c..3.............c^^^c..cc.........xxxxxxxxx");
    strcpy(vgrid[19], "xxxxxxxxx..........cc..c..c222c111c^c...............5c^^^c..cc.........xxxxxxxxx");
    strcpy(vgrid[20], "xxxxxxxxx..........cc..c..c2c222c.^^c......2.........=cccc..cc.........xxxxxxxxx");
    strcpy(vgrid[21], "xxxxxxxxx..........cc..c==ccccccccccc..........3......5..c^^cc.........xxxxxxxxx");
    strcpy(vgrid[22], "xxxxxxxxx..........cc..c.................................c..cc.........xxxxxxxxx");
    strcpy(vgrid[23], "xxxxxxxxx..........cc..c..........................3......c..cc.........xxxxxxxxx");
    strcpy(vgrid[24], "xxxxxxxxx..........cc^^cccccccccccccc.......2............c..cc.........xxxxxxxxx");
    strcpy(vgrid[25], "xxxxxxxxx..........cc..c............c....................c..cc.........xxxxxxxxx");
    strcpy(vgrid[26], "xxxxxxxxx..........cc..c............c.................3..c..cc.........xxxxxxxxx");
    strcpy(vgrid[27], "xxxxxxxxx..........cc..c..cccccccc..c..........2.........c^^cc.........xxxxxxxxx");
    strcpy(vgrid[28], "xxxxxxxxx..........cc5.c..c^^.c11c..c....................c..cc.........xxxxxxxxx");
    strcpy(vgrid[29], "xxxxxxxxx..........cc..c..c^c.c11c..c...3................c..cc.........xxxxxxxxx");
    strcpy(vgrid[30], "xxxxxxxxx..........cc..c..c^c.11cc..c..............2.....c..cc.........xxxxxxxxx");
    strcpy(vgrid[31], "xxxxxxxxx..........cc..c..c^cccccc..c.......2............c.5cc.........xxxxxxxxx");
    strcpy(vgrid[32], "xxxxxxxxx..........cc..c..c^^^^^^...c.................2..c..cc.........xxxxxxxxx");
    strcpy(vgrid[33], "xxxxxxxxx..........cc^^c==ccccccccccc....................c..cc.........xxxxxxxxx");
    strcpy(vgrid[34], "xxxxxxxxx..........cc..c.................................c..cc.........xxxxxxxxx");
    strcpy(vgrid[35], "xxxxxxxxx..........cc..c.................................c..cc.........xxxxxxxxx");
    strcpy(vgrid[36], "xxxxxxxxx..........cc..cccccccccccccc.^^^^^.cccccccccccccc..cc.........xxxxxxxxx");
    strcpy(vgrid[37], "xxxxxxxxx..........cc..c..^....^...cccc=c=cccc......^.^..c^^cc.........xxxxxxxxx");
    strcpy(vgrid[38], "xxxxxxxxx..........cc..c.^^....^...cc.2...2.cc......^.^..c..cc.........xxxxxxxxx");
    strcpy(vgrid[39], "xxxxxxxxx..........cc..c^^ccccccc..cc.F...F.cc..ccccccc..c..cc.........xxxxxxxxx");
    strcpy(vgrid[40], "xxxxxxxxx..........cc..c..cc.322c..cc.......cc..c22..cc..c..cc.........xxxxxxxxx");
    strcpy(vgrid[41], "xxxxxxxxx..........cc..c..c].c22c..cc.......cc..c22c.}c^^c..cc.........xxxxxxxxx");
    strcpy(vgrid[42], "xxxxxxxxx..........cc..c..cccc..c^^cc.G...G.cc..c3.cccc..c..cc.........xxxxxxxxx");
    strcpy(vgrid[43], "xxxxxxxxx..........cc..c.....^..c..cc.......cc^^c....^...c..cc.........xxxxxxxxx");
    strcpy(vgrid[44], "xxxxxxxxx..........cc..c.....^..c..cc.......cc..c....^...c..cc.........xxxxxxxxx");
    strcpy(vgrid[45], "xxxxxxxxx..........cc^^cccccccccc==cc.G...G.cc==cccccccccc^^cc.........xxxxxxxxx");
    strcpy(vgrid[46], "xxxxxxxxx..........cc......^....^..cc.......cc.....^....^...cc.........xxxxxxxxx");
    strcpy(vgrid[47], "xxxxxxxxx..........cc......^....^..cc.......cc.....^....^...cc.........xxxxxxxxx");
    strcpy(vgrid[48], "xxxxxxxxx..........cccccccccccccccccc.G...G.cccccccccccccccccc.........xxxxxxxxx");
    strcpy(vgrid[49], "xxxxxxxxx..........cccccccccccccccccc.......cccccccccccccccccc.........xxxxxxxxx");
    strcpy(vgrid[50], "xxxxxxxxx..............................................................xxxxxxxxx");
    strcpy(vgrid[51], "xxxxxxxxx.............................G...G............................xxxxxxxxx");
    strcpy(vgrid[52], "xxxxxxxxx...........................4.......4..........................xxxxxxxxx");
    strcpy(vgrid[53], "xxxxxxxxx..............................................................xxxxxxxxx");
    strcpy(vgrid[54], "xxxxxxxxx..............................................................xxxxxxxxx");
    strcpy(vgrid[55], "xxxxxxxxx...........................4..V.V..4..........................xxxxxxxxx");
    strcpy(vgrid[56], "xxxxxxxxx..............................................................xxxxxxxxx");
    strcpy(vgrid[57], "xxxxxxxxx..............................................................xxxxxxxxx");
    strcpy(vgrid[58], "xxxxxxxxx..............................................................xxxxxxxxx");
    strcpy(vgrid[59], "xxxxxxxxx...........................4.......4..........................xxxxxxxxx");
    strcpy(vgrid[60], "xxxxxxxxx..............................................................xxxxxxxxx");
    strcpy(vgrid[61], "xxxxxxxxx..............................................................xxxxxxxxx");
    strcpy(vgrid[62], "xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx");
    strcpy(vgrid[63], "xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx");
    strcpy(vgrid[64], "xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx");
    strcpy(vgrid[65], "xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx");
    strcpy(vgrid[66], "xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx");
    strcpy(vgrid[67], "xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx");
    strcpy(vgrid[68], "xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx");
    strcpy(vgrid[69], "xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx");

    mons_array[0] = MONS_MUMMY;
    mons_array[1] = MONS_GUARDIAN_MUMMY;
    mons_array[2] = MONS_MUMMY_PRIEST;
    mons_array[3] = MONS_SPHINX;
    mons_array[4] = MONS_GREATER_MUMMY;
    mons_array[5] = RANDOM_MONSTER;
    mons_array[6] = RANDOM_MONSTER;

    replace_with_func( vgrid, 70, 50, '^', trapped_area );

    return MAP_ENCOMPASS;
}


static char tomb_2(char vgrid[81][81], FixedVector<int, 7>& mons_array)
{

    strcpy(vgrid[0], "xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx");
    strcpy(vgrid[1], "xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx");
    strcpy(vgrid[2], "xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx");
    strcpy(vgrid[3], "xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx");
    strcpy(vgrid[4], "xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx");
    strcpy(vgrid[5], "xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx");
    strcpy(vgrid[6], "xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx");
    strcpy(vgrid[7], "xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx");
    strcpy(vgrid[8], "xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx");
    strcpy(vgrid[9], "xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx");
    strcpy(vgrid[10], "xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx");
    strcpy(vgrid[11], "xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx");
    strcpy(vgrid[12], "xxxxxxxxxxxxxxxxxxxcccccccccccccccccccccccccccccccccccccccccccxxxxxxxxxxxxxxxxxx");
    strcpy(vgrid[13], "xxxxxxxxxxxxxxxxxxxcccccccccccccccccccccccccccccccccccccccccccxxxxxxxxxxxxxxxxxx");
    strcpy(vgrid[14], "xxxxxxxxxxxxxxxxxxxcc{...c......c.....3....c........c.......ccxxxxxxxxxxxxxxxxxx");
    strcpy(vgrid[15], "xxxxxxxxxxxxxxxxxxxcc....c.....^c^........^c^.....2^c^......ccxxxxxxxxxxxxxxxxxx");
    strcpy(vgrid[16], "xxxxxxxxxxxxxxxxxxxcc....c...2.^=^.2.....2^=^..2...^=^......ccxxxxxxxxxxxxxxxxxx");
    strcpy(vgrid[17], "xxxxxxxxxxxxxxxxxxxcc.3.^c^....^c^........^c^2.....^c^......ccxxxxxxxxxxxxxxxxxx");
    strcpy(vgrid[18], "xxxxxxxxxxxxxxxxxxxcc..^^=^.....c..........c........c...2...ccxxxxxxxxxxxxxxxxxx");
    strcpy(vgrid[19], "xxxxxxxxxxxxxxxxxxxccccc=ccccccccccccccccccccccccccccccc....ccxxxxxxxxxxxxxxxxxx");
    strcpy(vgrid[20], "xxxxxxxxxxxxxxxxxxxcc..^^c.............................c....ccxxxxxxxxxxxxxxxxxx");
    strcpy(vgrid[21], "xxxxxxxxxxxxxxxxxxxcc....c.............................c..3.ccxxxxxxxxxxxxxxxxxx");
    strcpy(vgrid[22], "xxxxxxxxxxxxxxxxxxxcc....c..ccc4.................4ccc..c....ccxxxxxxxxxxxxxxxxxx");
    strcpy(vgrid[23], "xxxxxxxxxxxxxxxxxxxcc....c..ccc...................ccc..c..2.ccxxxxxxxxxxxxxxxxxx");
    strcpy(vgrid[24], "xxxxxxxxxxxxxxxxxxxcc....c..ccc.........1.........ccc..c)..}ccxxxxxxxxxxxxxxxxxx");
    strcpy(vgrid[25], "xxxxxxxxxxxxxxxxxxxcc.3..c..ccc.....2.......2.....ccc..cccccccxxxxxxxxxxxxxxxxxx");
    strcpy(vgrid[26], "xxxxxxxxxxxxxxxxxxxcc....c.............................c....ccxxxxxxxxxxxxxxxxxx");
    strcpy(vgrid[27], "xxxxxxxxxxxxxxxxxxxcc....c.............................c^2..ccxxxxxxxxxxxxxxxxxx");
    strcpy(vgrid[28], "xxxxxxxxxxxxxxxxxxxcc....c........c...........c........=^...ccxxxxxxxxxxxxxxxxxx");
    strcpy(vgrid[29], "xxxxxxxxxxxxxxxxxxxcc]...c.............................c^2..ccxxxxxxxxxxxxxxxxxx");
    strcpy(vgrid[30], "xxxxxxxxxxxxxxxxxxxccccccc.....3........(........3.....c....ccxxxxxxxxxxxxxxxxxx");
    strcpy(vgrid[31], "xxxxxxxxxxxxxxxxxxxcc....c.............................c.^^^ccxxxxxxxxxxxxxxxxxx");
    strcpy(vgrid[32], "xxxxxxxxxxxxxxxxxxxcc...^c........c...........c........ccc=cccxxxxxxxxxxxxxxxxxx");
    strcpy(vgrid[33], "xxxxxxxxxxxxxxxxxxxcc...^=.............................c.^^^ccxxxxxxxxxxxxxxxxxx");
    strcpy(vgrid[34], "xxxxxxxxxxxxxxxxxxxcc...^c.............................c....ccxxxxxxxxxxxxxxxxxx");
    strcpy(vgrid[35], "xxxxxxxxxxxxxxxxxxxcc....c..ccc.....2.......2.....ccc..c..2.ccxxxxxxxxxxxxxxxxxx");
    strcpy(vgrid[36], "xxxxxxxxxxxxxxxxxxxccccccc..ccc.........1.........ccc..c....ccxxxxxxxxxxxxxxxxxx");
    strcpy(vgrid[37], "xxxxxxxxxxxxxxxxxxxcc...^c..ccc...................ccc..c....ccxxxxxxxxxxxxxxxxxx");
    strcpy(vgrid[38], "xxxxxxxxxxxxxxxxxxxcc...^=..ccc4.................4ccc..c2...ccxxxxxxxxxxxxxxxxxx");
    strcpy(vgrid[39], "xxxxxxxxxxxxxxxxxxxcc...^c.............................c..2.ccxxxxxxxxxxxxxxxxxx");
    strcpy(vgrid[40], "xxxxxxxxxxxxxxxxxxxccccccc.............................c.^^^ccxxxxxxxxxxxxxxxxxx");
    strcpy(vgrid[41], "xxxxxxxxxxxxxxxxxxxcc...^c^............................ccc=cccxxxxxxxxxxxxxxxxxx");
    strcpy(vgrid[42], "xxxxxxxxxxxxxxxxxxxcc...^=cccc=ccccccccccccccc=ccccccccc.^^^ccxxxxxxxxxxxxxxxxxx");
    strcpy(vgrid[43], "xxxxxxxxxxxxxxxxxxxcc.1.^^^c.^^^c............c^^......c.3...ccxxxxxxxxxxxxxxxxxx");
    strcpy(vgrid[44], "xxxxxxxxxxxxxxxxxxxcc...2..c.1..c.....1.1....c.....2..c.....ccxxxxxxxxxxxxxxxxxx");
    strcpy(vgrid[45], "xxxxxxxxxxxxxxxxxxxcc......c....c..1......1.^c^.2.....c...2.ccxxxxxxxxxxxxxxxxxx");
    strcpy(vgrid[46], "xxxxxxxxxxxxxxxxxxxcc..3...c.1..c...1...1..1^=^.......c.....ccxxxxxxxxxxxxxxxxxx");
    strcpy(vgrid[47], "xxxxxxxxxxxxxxxxxxxcc......c....c[..........^c^......3c.....ccxxxxxxxxxxxxxxxxxx");
    strcpy(vgrid[48], "xxxxxxxxxxxxxxxxxxxcccccccccccccccccccccccccccccccccccccccccccxxxxxxxxxxxxxxxxxx");
    strcpy(vgrid[49], "xxxxxxxxxxxxxxxxxxxcccccccccccccccccccccccccccccccccccccccccccxxxxxxxxxxxxxxxxxx");
    strcpy(vgrid[50], "xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx");
    strcpy(vgrid[51], "xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx");
    strcpy(vgrid[52], "xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx");
    strcpy(vgrid[53], "xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx");
    strcpy(vgrid[54], "xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx");
    strcpy(vgrid[55], "xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx");
    strcpy(vgrid[56], "xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx");
    strcpy(vgrid[57], "xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx");
    strcpy(vgrid[58], "xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx");
    strcpy(vgrid[59], "xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx");
    strcpy(vgrid[60], "xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx");
    strcpy(vgrid[61], "xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx");
    strcpy(vgrid[62], "xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx");
    strcpy(vgrid[63], "xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx");
    strcpy(vgrid[64], "xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx");
    strcpy(vgrid[65], "xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx");
    strcpy(vgrid[66], "xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx");
    strcpy(vgrid[67], "xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx");
    strcpy(vgrid[68], "xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx");
    strcpy(vgrid[69], "xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx");

    mons_array[0] = MONS_MUMMY;
    mons_array[1] = MONS_GUARDIAN_MUMMY;
    mons_array[2] = MONS_MUMMY_PRIEST;
    mons_array[3] = MONS_GREATER_MUMMY;
    mons_array[4] = RANDOM_MONSTER;
    mons_array[5] = RANDOM_MONSTER;
    mons_array[6] = RANDOM_MONSTER;

    replace_with_func( vgrid, 70, 50, '^', trapped_area );

    return MAP_ENCOMPASS;
}


static char tomb_3(char vgrid[81][81], FixedVector<int, 7>& mons_array)
{

    strcpy(vgrid[0], "xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx");
    strcpy(vgrid[1], "xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx");
    strcpy(vgrid[2], "xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx");
    strcpy(vgrid[3], "xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx");
    strcpy(vgrid[4], "xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx");
    strcpy(vgrid[5], "xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx");
    strcpy(vgrid[6], "xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx");
    strcpy(vgrid[7], "xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx");
    strcpy(vgrid[8], "xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx");
    strcpy(vgrid[9], "xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx");
    strcpy(vgrid[10], "xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx");
    strcpy(vgrid[11], "xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx");
    strcpy(vgrid[12], "xxxxxxxxxxxxxxxxxxxcccccccccccccccccccccccccccccccccccccccccccxxxxxxxxxxxxxxxxxx");
    strcpy(vgrid[13], "xxxxxxxxxxxxxxxxxxxcccccccccccccccccccccccccccccccccccccccccccxxxxxxxxxxxxxxxxxx");
    strcpy(vgrid[14], "xxxxxxxxxxxxxxxxxxxccccccc.............................cccccccxxxxxxxxxxxxxxxxxx");
    strcpy(vgrid[15], "xxxxxxxxxxxxxxxxxxxcccc...............cccccc..............ccccxxxxxxxxxxxxxxxxxx");
    strcpy(vgrid[16], "xxxxxxxxxxxxxxxxxxxccc...............cccccccc..............cccxxxxxxxxxxxxxxxxxx");
    strcpy(vgrid[17], "xxxxxxxxxxxxxxxxxxxccc.......4......ccccO4cccc......4......cccxxxxxxxxxxxxxxxxxx");
    strcpy(vgrid[18], "xxxxxxxxxxxxxxxxxxxccc............cccc.^^^^.cccc...........cccxxxxxxxxxxxxxxxxxx");
    strcpy(vgrid[19], "xxxxxxxxxxxxxxxxxxxcc............cccc..^^^^..cccc...........ccxxxxxxxxxxxxxxxxxx");
    strcpy(vgrid[20], "xxxxxxxxxxxxxxxxxxxcc............cccc........cccc...........ccxxxxxxxxxxxxxxxxxx");
    strcpy(vgrid[21], "xxxxxxxxxxxxxxxxxxxcc...........cccc..444444..cccc..........ccxxxxxxxxxxxxxxxxxx");
    strcpy(vgrid[22], "xxxxxxxxxxxxxxxxxxxcc.......................................ccxxxxxxxxxxxxxxxxxx");
    strcpy(vgrid[23], "xxxxxxxxxxxxxxxxxxxcc.......................................ccxxxxxxxxxxxxxxxxxx");
    strcpy(vgrid[24], "xxxxxxxxxxxxxxxxxxxcc.................222222................ccxxxxxxxxxxxxxxxxxx");
    strcpy(vgrid[25], "xxxxxxxxxxxxxxxxxxxccc................223322...............cccxxxxxxxxxxxxxxxxxx");
    strcpy(vgrid[26], "xxxxxxxxxxxxxxxxxxxccc...3............223322............3..cccxxxxxxxxxxxxxxxxxx");
    strcpy(vgrid[27], "xxxxxxxxxxxxxxxxxxxcccc...............222222..............ccccxxxxxxxxxxxxxxxxxx");
    strcpy(vgrid[28], "xxxxxxxxxxxxxxxxxxxcccc....2..........................2...ccccxxxxxxxxxxxxxxxxxx");
    strcpy(vgrid[29], "xxxxxxxxxxxxxxxxxxxcccccc....2......................2....cccccxxxxxxxxxxxxxxxxxx");
    strcpy(vgrid[30], "xxxxxxxxxxxxxxxxxxxcccccccc............................cccccccxxxxxxxxxxxxxxxxxx");
    strcpy(vgrid[31], "xxxxxxxxxxxxxxxxxxxccccccccc=ccc..................ccc=ccccccccxxxxxxxxxxxxxxxxxx");
    strcpy(vgrid[32], "xxxxxxxxxxxxxxxxxxxcccccccc....cc................cc....cccccccxxxxxxxxxxxxxxxxxx");
    strcpy(vgrid[33], "xxxxxxxxxxxxxxxxxxxcccccc..^^...cc22222222222222cc..^^...cccccxxxxxxxxxxxxxxxxxx");
    strcpy(vgrid[34], "xxxxxxxxxxxxxxxxxxxcccc.^^..^^...cc............cc..^^.....ccccxxxxxxxxxxxxxxxxxx");
    strcpy(vgrid[35], "xxxxxxxxxxxxxxxxxxxcccc.^^^.......cc..........cc.....^^^^^ccccxxxxxxxxxxxxxxxxxx");
    strcpy(vgrid[36], "xxxxxxxxxxxxxxxxxxxccc~...^^.......cc........cc......^^...~cccxxxxxxxxxxxxxxxxxx");
    strcpy(vgrid[37], "xxxxxxxxxxxxxxxxxxxccc~~~..^^.......cc222222cc....^^^^..~~~cccxxxxxxxxxxxxxxxxxx");
    strcpy(vgrid[38], "xxxxxxxxxxxxxxxxxxxccc~~~~..^^.......c......c.....^^...~~~~cccxxxxxxxxxxxxxxxxxx");
    strcpy(vgrid[39], "xxxxxxxxxxxxxxxxxxxccc~~~~~~.^^^.....c......c....^^..~~~~~~cccxxxxxxxxxxxxxxxxxx");
    strcpy(vgrid[40], "xxxxxxxxxxxxxxxxxxxccc~~~~~~~.^^.....c......c...^^..~~~~~~~cccxxxxxxxxxxxxxxxxxx");
    strcpy(vgrid[41], "xxxxxxxxxxxxxxxxxxxcccc~~~~~~..^.....c......c...^...~~~~~~ccccxxxxxxxxxxxxxxxxxx");
    strcpy(vgrid[42], "xxxxxxxxxxxxxxxxxxxccccc~~~~~~.^^....c......c...^..~~~~~~cccccxxxxxxxxxxxxxxxxxx");
    strcpy(vgrid[43], "xxxxxxxxxxxxxxxxxxxcccccc~~~~~..^....c......c.^^^..~~~~~ccccccxxxxxxxxxxxxxxxxxx");
    strcpy(vgrid[44], "xxxxxxxxxxxxxxxxxxxccccccc~~~~~.^^...c......c.^.^.~~~~~cccccccxxxxxxxxxxxxxxxxxx");
    strcpy(vgrid[45], "xxxxxxxxxxxxxxxxxxxcccccccc~~~~.^^..cc..{...cc.^^.~~~~ccccccccxxxxxxxxxxxxxxxxxx");
    strcpy(vgrid[46], "xxxxxxxxxxxxxxxxxxxccccccccc~~~.^^..cc...(..cc.^..~~~cccccccccxxxxxxxxxxxxxxxxxx");
    strcpy(vgrid[47], "xxxxxxxxxxxxxxxxxxxcccccccccc~~.^^cccc..[...cccc..~~ccccccccccxxxxxxxxxxxxxxxxxx");
    strcpy(vgrid[48], "xxxxxxxxxxxxxxxxxxxcccccccccccccccccccccccccccccccccccccccccccxxxxxxxxxxxxxxxxxx");
    strcpy(vgrid[49], "xxxxxxxxxxxxxxxxxxxcccccccccccccccccccccccccccccccccccccccccccxxxxxxxxxxxxxxxxxx");
    strcpy(vgrid[50], "xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx");
    strcpy(vgrid[51], "xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx");
    strcpy(vgrid[52], "xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx");
    strcpy(vgrid[53], "xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx");
    strcpy(vgrid[54], "xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx");
    strcpy(vgrid[55], "xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx");
    strcpy(vgrid[56], "xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx");
    strcpy(vgrid[57], "xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx");
    strcpy(vgrid[58], "xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx");
    strcpy(vgrid[59], "xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx");
    strcpy(vgrid[60], "xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx");
    strcpy(vgrid[61], "xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx");
    strcpy(vgrid[62], "xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx");
    strcpy(vgrid[63], "xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx");
    strcpy(vgrid[64], "xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx");
    strcpy(vgrid[65], "xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx");
    strcpy(vgrid[66], "xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx");
    strcpy(vgrid[67], "xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx");
    strcpy(vgrid[68], "xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx");
    strcpy(vgrid[69], "xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx");

    mons_array[0] = MONS_MUMMY;
    mons_array[1] = MONS_GUARDIAN_MUMMY;
    mons_array[2] = MONS_MUMMY_PRIEST;
    mons_array[3] = MONS_GREATER_MUMMY;
    mons_array[4] = RANDOM_MONSTER;
    mons_array[5] = RANDOM_MONSTER;
    mons_array[6] = RANDOM_MONSTER;

    replace_with_func( vgrid, 70, 50, '~', good_treasure_area );
    replace_with_func( vgrid, 70, 50, '^', trapped_area );

    return MAP_ENCOMPASS;
}



static char swamp(char vgrid[81][81], FixedVector<int, 7>& mons_array)
{     // NB - most of the 'x's here will be set to water in dungeon.cc

    for (unsigned char i = 0; i < 81; i++)
        strcpy(vgrid[i], "xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx");

    strcat(vgrid[36], "xxxxxxxxxxx@xxxxxxxxxxxxxxxxxxxxxxxxxxxx");
    strcat(vgrid[37], "xxxxxxxxxxx2xxxxxxxxxxxxxxxxxxxxxxxxxxxx");
    strcat(vgrid[38], "xxxxxxxxxx.xxxxxxxxxxxxxxxxxxxxxxxxxxxxx");
    strcat(vgrid[39], "xxxxxxxxxxx.xxxxxxxxxxxxxxxxxxxxxxxxxxxx");
    strcat(vgrid[40], "xxxxxxxxxx2x.xxxxxxxxxxxxxxxxxxxxxxxxxxx");
    strcat(vgrid[41], "xxxxxxxxxxxx.xxxxxxxxxxxxxxxxxxxxxxxxxxx");
    strcat(vgrid[42], "xxxxxxxxxcc.ccxxxxxxxxxxxxxxxxxxxxxxxxxx");
    strcat(vgrid[43], "xxxxxxxxcc...ccxxxxxxxxxxxxxxxxxxxxxxxxx");
    strcat(vgrid[44], "xxxxxxxcc3.2..ccxxxxxxxxxxxxxxxxxxxxxxxx");
    strcat(vgrid[45], "xxxxxxcc.1.3.2.ccxxxxxxxxxxxxxxxxxxxxxxx");
    strcat(vgrid[46], "xxxxxccc....1.1cccxxxxxxxxxxxxxxxxxxxxxx");
    strcat(vgrid[47], "xxxxxcc.1.32....ccxxxxxxxxxxxxxxxxxxxxxx");
    strcat(vgrid[48], "xxxxxcc...3..1.3ccxxxxxxxxxxxxxxxxxxxxxx");
    strcat(vgrid[49], "xxxxxcc2.1.3..2.ccxxxxxxxxxxxxxxxxxxxxxx");
    strcat(vgrid[50], "xxxxxccc33..1..cccxxxxxxxxxxxxxxxxxxxxxx");
    strcat(vgrid[51], "xxxxxxcccc3O3ccccxxxxxxxxxxxxxxxxxxxxxxx");
    strcat(vgrid[52], "xxxxxxxcccccccccxxxxxxxxxxxxxxxxxxxxxxxx");
    strcat(vgrid[53], "xxxxxxxxcccccccxxxxxxxxxxxxxxxxxxxxxxxxx");
    strcat(vgrid[54], "xxxxxxxxxxcccxxxxxxxxxxxxxxxxxxxxxxxxxxx");
    strcat(vgrid[55], "xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx");
    strcat(vgrid[56], "xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx");
    strcat(vgrid[57], "xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx");
    strcat(vgrid[58], "xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx");
    strcat(vgrid[59], "xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx");
    strcat(vgrid[60], "xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx");
    strcat(vgrid[61], "xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx");
    strcat(vgrid[62], "xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx");
    strcat(vgrid[63], "xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx");
    strcat(vgrid[64], "xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx");
    strcat(vgrid[65], "xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx");
    strcat(vgrid[66], "xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx");
    strcat(vgrid[67], "xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx");
    strcat(vgrid[68], "xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx");
    strcat(vgrid[69], "xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx");

    mons_array[0] = MONS_SWAMP_DRAGON;
    mons_array[1] = MONS_SWAMP_DRAKE;
    mons_array[2] = MONS_HYDRA;
    mons_array[3] = RANDOM_MONSTER;
    mons_array[4] = RANDOM_MONSTER;
    mons_array[5] = RANDOM_MONSTER;
    mons_array[6] = RANDOM_MONSTER;

    return MAP_SOUTHEAST;
}


/*
   NOTE: *Cannot* place 8,9 or 0 monsters in branch vaults which neither use the
   normal mons_level function or are around level 35, or generation will crash.
*/

static char minivault_1(char vgrid[81][81], FixedVector<int, 7>& mons_array)
{
    UNUSED( mons_array );

    strcpy(vgrid[0],  "............");
    strcpy(vgrid[1],  "..xxxx=xxx..");
    strcpy(vgrid[2],  ".xx..x...xx.");
    strcpy(vgrid[3],  ".x.0..x.0.x.");
    strcpy(vgrid[4],  ".x...x....x.");
    strcpy(vgrid[5],  ".xx.x*x.x.=.");
    strcpy(vgrid[6],  ".=.x.x*x.xx.");
    strcpy(vgrid[7],  ".x....x...x.");
    strcpy(vgrid[8],  ".x.0.x..0.x.");
    strcpy(vgrid[9],  ".xx...x..xx.");
    strcpy(vgrid[10], "..xxx=xxxx..");
    strcpy(vgrid[11], "............");

    return MAP_NORTH;
}


static char minivault_2(char vgrid[81][81], FixedVector<int, 7>& mons_array)
{
    UNUSED( mons_array );

    strcpy(vgrid[0],  "............");
    strcpy(vgrid[1],  "..xxxx.xxxx.");
    strcpy(vgrid[2],  "..xx.....xx.");
    strcpy(vgrid[3],  "..x.......x.");
    strcpy(vgrid[4],  "..x.......x.");
    strcpy(vgrid[5],  "......C.....");
    strcpy(vgrid[6],  "..x.......x.");
    strcpy(vgrid[7],  "..x.......x.");
    strcpy(vgrid[8],  "..xx.....xx.");
    strcpy(vgrid[9],  "..xxxx.xxxx.");
    strcpy(vgrid[10], "............");
    strcpy(vgrid[11], "............");

    return MAP_NORTH;
}


static char minivault_3(char vgrid[81][81], FixedVector<int, 7>& mons_array)
{
    UNUSED( mons_array );

    strcpy(vgrid[0],  "............");
    strcpy(vgrid[1],  ".cccccccccc.");
    strcpy(vgrid[2],  ".cccccccccc.");
    strcpy(vgrid[3],  ".cBcBcBcBcc.");
    strcpy(vgrid[4],  ".G.c.c.c.Bc.");
    strcpy(vgrid[5],  ".........Bc.");
    strcpy(vgrid[6],  ".........Bc.");
    strcpy(vgrid[7],  ".G.c.c.c.Bc.");
    strcpy(vgrid[8],  ".cBcBcBcBcc.");
    strcpy(vgrid[9],  ".cccccccccc.");
    strcpy(vgrid[10], ".cccccccccc.");
    strcpy(vgrid[11], "............");

    return MAP_NORTH;
}


static char minivault_4(char vgrid[81][81], FixedVector<int, 7>& mons_array)
{
    UNUSED( mons_array );

    strcpy(vgrid[0],  "............");
    strcpy(vgrid[1],  "....xwxx....");
    strcpy(vgrid[2],  "..xxxwwxwx..");
    strcpy(vgrid[3],  "..xwwwwwwx..");
    strcpy(vgrid[4],  ".xwwxwwxwxx.");
    strcpy(vgrid[5],  ".xwwwwwwwwx.");
    strcpy(vgrid[6],  ".xwwxwwwxww.");
    strcpy(vgrid[7],  ".xxwwwwwwxx.");
    strcpy(vgrid[8],  "..wwwwxwwx..");
    strcpy(vgrid[9],  "..xxxwwxxw..");
    strcpy(vgrid[10], "....xxww....");
    strcpy(vgrid[11], "............");

    return MAP_NORTH;
}


static char minivault_5(char vgrid[81][81], FixedVector<int, 7>& mons_array)
{
    UNUSED( mons_array );

    strcpy(vgrid[0],  "............");
    strcpy(vgrid[1],  ".x=xxxxxxxx.");
    strcpy(vgrid[2],  ".x.x0.....x.");
    strcpy(vgrid[3],  ".x.x.xxxx.x.");
    strcpy(vgrid[4],  ".x.x.x**x.x.");
    strcpy(vgrid[5],  ".x.x.x**x.x.");
    strcpy(vgrid[6],  ".x.x.xx8x.x.");
    strcpy(vgrid[7],  ".x.x9...x.x.");
    strcpy(vgrid[8],  ".x.xxxxxx.x.");
    strcpy(vgrid[9],  ".x........x.");
    strcpy(vgrid[10], ".xxxxxxxxxx.");
    strcpy(vgrid[11], "............");

    return MAP_NORTH;
}


static char minivault_6(char vgrid[81][81], FixedVector<int, 7>& mons_array)
{     // Wizard's laboratory

    strcpy(vgrid[0],  "............");
    strcpy(vgrid[1],  ".cCCCCCCCCc.");
    strcpy(vgrid[2],  ".C........c.");
    strcpy(vgrid[3],  ".C........c.");
    strcpy(vgrid[4],  ".C..1111..c.");
    strcpy(vgrid[5],  ".C........c.");
    strcpy(vgrid[6],  ".cccccAAAAc.");
    strcpy(vgrid[7],  ".c~~~B3232c.");
    strcpy(vgrid[8],  ".c~~~B2223c.");
    strcpy(vgrid[9],  ".c~~~B3322c.");
    strcpy(vgrid[10], ".cccccccccc.");
    strcpy(vgrid[11], "............");

    mons_array[0] = MONS_WIZARD;
    mons_array[1] = MONS_ABOMINATION_SMALL;
    mons_array[2] = MONS_ABOMINATION_LARGE;

    replace_one_of( vgrid, 12, 12, 'A', '=', 'c' );
    replace_one_of( vgrid, 12, 12, 'B', '=', 'c' );
    replace_one_of( vgrid, 12, 12, 'C', '=', 'c' );
    replace_one_of( vgrid, 12, 12, '1', '1', '.' );

    replace_with_func( vgrid, 12, 12, '~', good_treasure_area );

    return MAP_NORTH;
}


static char minivault_7(char vgrid[81][81], FixedVector<int, 7>& mons_array)
{     // beehive minivault

    strcpy(vgrid[0],  "............");
    strcpy(vgrid[1],  "....aaaa....");
    strcpy(vgrid[2],  "..a2a2aaaa..");
    strcpy(vgrid[3],  "..aaRa3a2a..");
    strcpy(vgrid[4],  ".aa2aRa2aaa.");
    strcpy(vgrid[5],  ".a3aRa1aRa2.");
    strcpy(vgrid[6],  ".aa3aRaRa2a.");
    strcpy(vgrid[7],  ".aaa2a2a3aa.");
    strcpy(vgrid[8],  "..a3aRa2aa..");
    strcpy(vgrid[9],  "...aa2aa2a..");
    strcpy(vgrid[10], "....aaaa....");
    strcpy(vgrid[11], "............");

    mons_array[0] = MONS_QUEEN_BEE;
    mons_array[1] = MONS_KILLER_BEE;
    mons_array[2] = MONS_KILLER_BEE_LARVA;

    return MAP_NORTH;
}


static char minivault_8(char vgrid[81][81], FixedVector<int, 7>& mons_array)
{     // lava pond

    strcpy(vgrid[0],  "x.x.x.x.x.x.");
    strcpy(vgrid[1],  ".c.c.c.c.c.x");
    strcpy(vgrid[2],  "x...l1l...c.");
    strcpy(vgrid[3],  ".c.llllll..x");
    strcpy(vgrid[4],  "x.lllllll1c.");
    strcpy(vgrid[5],  ".c.llFGll..x");
    strcpy(vgrid[6],  "x..llGFll.c.");
    strcpy(vgrid[7],  ".c1lllllll.x");
    strcpy(vgrid[8],  "x..llllll.c.");
    strcpy(vgrid[9],  ".c...l1l...x");
    strcpy(vgrid[10], "x.c.c.c.c.c.");
    strcpy(vgrid[11], ".x.x.x.x.x.x");

    mons_array[0] = MONS_MOLTEN_GARGOYLE;

    return MAP_NORTH;
}


static char minivault_9(char vgrid[81][81], FixedVector<int, 7>& mons_array)
{     // evil zoo
    UNUSED( mons_array );

    strcpy(vgrid[0],  "............");
    strcpy(vgrid[1],  ".x=x=x=x=x=.");
    strcpy(vgrid[2],  ".=x=x=x=x=x.");
    strcpy(vgrid[3],  ".x=x=x=x=x=.");
    strcpy(vgrid[4],  ".=x=8888x=x.");
    strcpy(vgrid[5],  ".x=x8998=x=.");
    strcpy(vgrid[6],  ".=x=8998x=x.");
    strcpy(vgrid[7],  ".x=x8888=x=.");
    strcpy(vgrid[8],  ".=x=x=x=x=x.");
    strcpy(vgrid[9],  ".x=x=x=x=x=.");
    strcpy(vgrid[10], ".=x=x=x=x=x.");
    strcpy(vgrid[11], "............");

    return MAP_NORTH;

}


static char minivault_10(char vgrid[81][81], FixedVector<int, 7>& mons_array)
{
    UNUSED( mons_array );

    strcpy(vgrid[0],  "............");
    strcpy(vgrid[1],  ".cAAc..cBBc.");
    strcpy(vgrid[2],  ".A**A..B**B.");
    strcpy(vgrid[3],  ".A**A..B**B.");
    strcpy(vgrid[4],  ".cAAc..cBBc.");
    strcpy(vgrid[5],  ".....90.....");
    strcpy(vgrid[6],  ".....09.....");
    strcpy(vgrid[7],  ".cCCc..cDDc.");
    strcpy(vgrid[8],  ".C**C..D**D.");
    strcpy(vgrid[9],  ".C**C..D**D.");
    strcpy(vgrid[10], ".cCCc..cDDc.");
    strcpy(vgrid[11], "............");

    replace_one_of( vgrid, 12, 12, 'A', '=', 'c' );  // room 1
    replace_one_of( vgrid, 12, 12, 'B', '=', 'c' );  // room 2
    replace_one_of( vgrid, 12, 12, 'C', '=', 'c' );  // room 3
    replace_one_of( vgrid, 12, 12, 'D', '=', 'c' );  // room 4

    replace_one_of( vgrid, 12, 12, '*', '|', '*' );  // treasure

    return MAP_NORTH;
}


static char minivault_11(char vgrid[81][81], FixedVector<int, 7>& mons_array)
{     // multicoloured onion
    UNUSED( mons_array );

    strcpy(vgrid[0],  "............");
    strcpy(vgrid[1],  ".=xxxxxxxx=.");
    strcpy(vgrid[2],  ".x........x.");
    strcpy(vgrid[3],  ".x.=cccc=.x.");
    strcpy(vgrid[4],  ".x.c....c.x.");
    strcpy(vgrid[5],  ".x.c.bb.c.x.");
    strcpy(vgrid[6],  ".x.c.bb.c.x.");
    strcpy(vgrid[7],  ".x.c....c.x.");
    strcpy(vgrid[8],  ".x.=cccc=.x.");
    strcpy(vgrid[9],  ".x........x.");
    strcpy(vgrid[10], ".=xxxxxxxx=.");
    strcpy(vgrid[11], "............");

    return MAP_NORTH;
}


static char minivault_12(char vgrid[81][81], FixedVector<int, 7>& mons_array)
{     // closed box minivault
    UNUSED( mons_array );

    strcpy(vgrid[0],  "............");
    strcpy(vgrid[1],  ".cccccccccc.");
    strcpy(vgrid[2],  ".c>9.8.9.<c.");
    strcpy(vgrid[3],  ".c..9$9$..c.");
    strcpy(vgrid[4],  ".c.c****$.c.");
    strcpy(vgrid[5],  ".c.$****c.c.");
    strcpy(vgrid[6],  ".c.c****$.c.");
    strcpy(vgrid[7],  ".c.$****c.c.");
    strcpy(vgrid[8],  ".c.9$8$9..c.");
    strcpy(vgrid[9],  ".c<.9.9..>c.");
    strcpy(vgrid[10], ".cccccccccc.");
    strcpy(vgrid[11], "............");

    replace_one_of( vgrid, 12, 12, '*', '|', '*' );  // treasure

    return MAP_NORTH;
}


static char minivault_13(char vgrid[81][81], FixedVector<int, 7>& mons_array)
{     // little trap spiral
    UNUSED( mons_array );

    strcpy(vgrid[0],  "............");
    strcpy(vgrid[1],  ".cAAccccccc.");
    strcpy(vgrid[2],  ".A~~=~~c~~c.");
    strcpy(vgrid[3],  ".A~9c~~=9~c.");
    strcpy(vgrid[4],  ".cccccccc=c.");
    strcpy(vgrid[5],  ".c.8=BBc8.c.");
    strcpy(vgrid[6],  ".c8$cBBc$8c.");
    strcpy(vgrid[7],  ".c=cccccc=c.");
    strcpy(vgrid[8],  ".c~9c~~=9~c.");
    strcpy(vgrid[9],  ".c~~=~~c~~c.");
    strcpy(vgrid[10], ".cccccccccc.");
    strcpy(vgrid[11], "............");

    replace_one_of( vgrid, 12, 12, 'A', '=', 'c' );  // entrance
    replace_one_of( vgrid, 12, 12, 'B', '8', '|' );  // final room
    replace_with_func( vgrid, 12, 12, '~', trapped_area );

    return MAP_NORTH;
}


static char minivault_14(char vgrid[81][81], FixedVector<int, 7>& mons_array)
{     // water cross
    UNUSED( mons_array );

    strcpy(vgrid[0],  "............");
    strcpy(vgrid[1],  ".wwwww.wwww.");
    strcpy(vgrid[2],  ".wwwww.wwww.");
    strcpy(vgrid[3],  ".wwwww.wwww.");
    strcpy(vgrid[4],  ".wwwww.wwww.");
    strcpy(vgrid[5],  ".....8.wwww.");
    strcpy(vgrid[6],  ".wwww.......");
    strcpy(vgrid[7],  ".wwww.wwwww.");
    strcpy(vgrid[8],  ".wwww.wwwww.");
    strcpy(vgrid[9],  ".wwww.wwwww.");
    strcpy(vgrid[10], ".wwww.wwwww.");
    strcpy(vgrid[11], "............");

    return MAP_NORTH;
}

static char minivault_15(char vgrid[81][81], FixedVector<int, 7>& mons_array) /* lava pond */
{
    UNUSED( mons_array );

    strcpy(vgrid[0],  "............");
    strcpy(vgrid[1],  "............");
    strcpy(vgrid[2],  "....lll.....");
    strcpy(vgrid[3],  "...vvlvv....");
    strcpy(vgrid[4],  "..lv|*|vl...");
    strcpy(vgrid[5],  "..ll*S*ll...");
    strcpy(vgrid[6],  "..lv|*|vl...");
    strcpy(vgrid[7],  "...vvlvv....");
    strcpy(vgrid[8],  "....lll.....");
    strcpy(vgrid[9],  "............");
    strcpy(vgrid[10], "............");
    strcpy(vgrid[11], "............");

    return 1;
}


static char minivault_16(char vgrid[81][81], FixedVector<int, 7>& mons_array)
{
    UNUSED( mons_array );

    strcpy(vgrid[0],  "............");
    strcpy(vgrid[1],  "............");
    strcpy(vgrid[2],  "............");
    strcpy(vgrid[3],  "...v.....v..");
    strcpy(vgrid[4],  ".....www....");
    strcpy(vgrid[5],  "....ww|ww...");
    strcpy(vgrid[6],  "....w*S*w...");
    strcpy(vgrid[7],  "....ww|ww...");
    strcpy(vgrid[8],  ".....www....");
    strcpy(vgrid[9],  "...v.....v..");
    strcpy(vgrid[10], "............");
    strcpy(vgrid[11], "............");

    return MAP_NORTH;
}


static char minivault_17(char vgrid[81][81], FixedVector<int, 7>& mons_array)
{     // lava pond
    UNUSED( mons_array );

    strcpy(vgrid[0],  "............");
    strcpy(vgrid[1],  "............");
    strcpy(vgrid[2],  "............");
    strcpy(vgrid[3],  "............");
    strcpy(vgrid[4],  "............");
    strcpy(vgrid[5],  ".....F......");
    strcpy(vgrid[6],  "............");
    strcpy(vgrid[7],  "............");
    strcpy(vgrid[8],  "............");
    strcpy(vgrid[9],  "............");
    strcpy(vgrid[10], "............");
    strcpy(vgrid[11], "............");

    return MAP_NORTH;
}


static char minivault_18(char vgrid[81][81], FixedVector<int, 7>& mons_array)
{     // lava pond
    UNUSED( mons_array );

    strcpy(vgrid[0], "............");
    strcpy(vgrid[1], "............");
    strcpy(vgrid[2], "............");
    strcpy(vgrid[3], "...blllb....");
    strcpy(vgrid[4], "...ll*ll....");
    strcpy(vgrid[5], "...l|H|l....");
    strcpy(vgrid[6], "...ll*ll....");
    strcpy(vgrid[7], "...blllb....");
    strcpy(vgrid[8], "............");
    strcpy(vgrid[9], "............");
    strcpy(vgrid[10], "............");
    strcpy(vgrid[11], "............");

    return MAP_NORTH;
}


static char minivault_19(char vgrid[81][81], FixedVector<int, 7>& mons_array)
{
    UNUSED( mons_array );

    strcpy(vgrid[0],  "............");
    strcpy(vgrid[1],  ".xx......xx.");
    strcpy(vgrid[2],  ".xxx.0..xxx.");
    strcpy(vgrid[3],  "..xxx..xxx..");
    strcpy(vgrid[4],  "...xxxxxx...");
    strcpy(vgrid[5],  "....x$$x.0..");
    strcpy(vgrid[6],  "..0.x$$x....");
    strcpy(vgrid[7],  "...xxxxxx...");
    strcpy(vgrid[8],  "..xxx..xxx..");
    strcpy(vgrid[9],  ".xxx..0.xxx.");
    strcpy(vgrid[10], ".xx......xx.");
    strcpy(vgrid[11], "............");

    return MAP_NORTH;
}


static char minivault_20(char vgrid[81][81], FixedVector<int, 7>& mons_array)
{     // lava pond
    UNUSED( mons_array );

    strcpy(vgrid[0],  "............");
    strcpy(vgrid[1],  ".xxxx~~xxxx.");
    strcpy(vgrid[2],  ".x........x.");
    strcpy(vgrid[3],  ".x.AAAAAA.x.");
    strcpy(vgrid[4],  ".x.A9..9A.x.");
    strcpy(vgrid[5],  ".~.A.bb.A.~.");
    strcpy(vgrid[6],  ".~.A.bb.A.~.");
    strcpy(vgrid[7],  ".x.A9..9A.x.");
    strcpy(vgrid[8],  ".x.AAAAAA.x.");
    strcpy(vgrid[9],  ".x........x.");
    strcpy(vgrid[10], ".xxxx~~xxxx.");
    strcpy(vgrid[11], "............");

    replace_one_of( vgrid, 12, 12, 'A', '=', 'v' );
    replace_one_of( vgrid, 12, 12, '9', '8', '9' );
    replace_with_func( vgrid, 12, 12, '~', trapped_area );

    return MAP_NORTH;
}


static char minivault_21(char vgrid[81][81], FixedVector<int, 7>& mons_array)
{
    UNUSED( mons_array );

    strcpy(vgrid[0],  "............");
    strcpy(vgrid[1],  ".^xxxxxxxx^.");
    strcpy(vgrid[2],  ".x........x.");
    strcpy(vgrid[3],  ".x.cccccc.x.");
    strcpy(vgrid[4],  ".x.c%%9<c.x.");
    strcpy(vgrid[5],  ".x.c%**9c.x.");
    strcpy(vgrid[6],  ".x.c9**%c.x.");
    strcpy(vgrid[7],  ".x.c>9%%c.x.");
    strcpy(vgrid[8],  ".x.cccccc.x.");
    strcpy(vgrid[9],  ".x........x.");
    strcpy(vgrid[10], ".^xxxxxxxx^.");
    strcpy(vgrid[11], "............");

    replace_one_of( vgrid, 12, 12, '9', '8', '9' );
    replace_one_of( vgrid, 12, 12, '*', '|', '*' );

    return MAP_NORTH;

}


static char minivault_22(char vgrid[81][81], FixedVector<int, 7>& mons_array)
{
    UNUSED( mons_array );

    strcpy(vgrid[0],  "............");
    strcpy(vgrid[1],  ".....cc.....");
    strcpy(vgrid[2],  "...cccccc...");
    strcpy(vgrid[3],  "..c^c00c^c..");
    strcpy(vgrid[4],  "..cc=cc=cc..");
    strcpy(vgrid[5],  ".cc0c$$c0cc.");
    strcpy(vgrid[6],  ".cc0c$$c0cc.");
    strcpy(vgrid[7],  "..cc=cc=cc..");
    strcpy(vgrid[8],  "..c^c00c^c..");
    strcpy(vgrid[9],  "...cccccc...");
    strcpy(vgrid[10], ".....cc.....");
    strcpy(vgrid[11], "............");

    return MAP_NORTH;

}


static char minivault_23(char vgrid[81][81], FixedVector<int, 7>& mons_array)
{
    UNUSED( mons_array );

    strcpy(vgrid[0],  "x.x.x.x.x.x.");
    strcpy(vgrid[1],  ".x.x.x.x.x.x");
    strcpy(vgrid[2],  "x.x0x%x.x.x.");
    strcpy(vgrid[3],  ".x.x.x.x0x.x");
    strcpy(vgrid[4],  "x.x.x.x.x.x.");
    strcpy(vgrid[5],  ".x.x.x.x.x.x");
    strcpy(vgrid[6],  "x.x9x.x%x.x.");
    strcpy(vgrid[7],  ".x.x.x.x.x.x");
    strcpy(vgrid[8],  "x.x%x.x.x.x.");
    strcpy(vgrid[9],  ".x.x.x.x0x.x");
    strcpy(vgrid[10], "x.x.x.x.x.x.");
    strcpy(vgrid[11], ".x.x.x.x.x.x");

    return MAP_NORTH;

}


static char minivault_24(char vgrid[81][81], FixedVector<int, 7>& mons_array)
{
    UNUSED( mons_array );

    strcpy(vgrid[0],  "............");
    strcpy(vgrid[1],  "....cccc....");
    strcpy(vgrid[2],  "....c9%c....");
    strcpy(vgrid[3],  "....c=cc....");
    strcpy(vgrid[4],  ".cccc8cAccc.");
    strcpy(vgrid[5],  ".c9=0c%cc%c.");
    strcpy(vgrid[6],  ".c%cc%c0=9c.");
    strcpy(vgrid[7],  ".cccAc8cccc.");
    strcpy(vgrid[8],  "....cc=c....");
    strcpy(vgrid[9],  "....c%9c....");
    strcpy(vgrid[10], "....cccc....");
    strcpy(vgrid[11], "............");

    replace_one_of( vgrid, 12, 12, 'A', '=', 'c' );

    return MAP_NORTH;

}


static char minivault_25(char vgrid[81][81], FixedVector<int, 7>& mons_array)
{
    UNUSED( mons_array );

    strcpy(vgrid[0],  "............");
    strcpy(vgrid[1],  ".xxAxxxxxxx.");
    strcpy(vgrid[2],  ".x........x.");
    strcpy(vgrid[3],  ".x..%..%..A.");
    strcpy(vgrid[4],  ".x.....0..x.");
    strcpy(vgrid[5],  ".x...%....x.");
    strcpy(vgrid[6],  ".x..9..%..x.");
    strcpy(vgrid[7],  ".x..%.....x.");
    strcpy(vgrid[8],  ".A....%0..x.");
    strcpy(vgrid[9],  ".x........x.");
    strcpy(vgrid[10], ".xxxxxxxAxx.");
    strcpy(vgrid[11], "............");

    replace_one_of( vgrid, 12, 12, 'A', '=', 'x' );

    return MAP_NORTH;

}


static char minivault_26(char vgrid[81][81], FixedVector<int, 7>& mons_array)
{
    UNUSED( mons_array );

    strcpy(vgrid[0],  "c..........c");
    strcpy(vgrid[1],  ".c...cc...c.");
    strcpy(vgrid[2],  "..c.9cc9.c..");
    strcpy(vgrid[3],  "...c....c...");
    strcpy(vgrid[4],  "..9.c%%c.9..");
    strcpy(vgrid[5],  ".cc.%cc%.cc.");
    strcpy(vgrid[6],  ".cc.%cc%.cc.");
    strcpy(vgrid[7],  "..9.c%%c.9..");
    strcpy(vgrid[8],  "...c....c...");
    strcpy(vgrid[9],  "..c.9cc9.c..");
    strcpy(vgrid[10], ".c...cc...c.");
    strcpy(vgrid[11], "c..........c");

    return MAP_NORTH;

}


static char minivault_27(char vgrid[81][81], FixedVector<int, 7>& mons_array)
{
    UNUSED( mons_array );

    strcpy(vgrid[0],  "............");
    strcpy(vgrid[1],  ".x=xxxxxxxx.");
    strcpy(vgrid[2],  ".x........x.");
    strcpy(vgrid[3],  ".xxxxxxxx=x.");
    strcpy(vgrid[4],  ".x%9....9%x.");
    strcpy(vgrid[5],  ".x=xxxxxxxx.");
    strcpy(vgrid[6],  ".x%9....9%x.");
    strcpy(vgrid[7],  ".xxxxxxxx=x.");
    strcpy(vgrid[8],  ".x........x.");
    strcpy(vgrid[9],  ".x=xxxxxxxx.");
    strcpy(vgrid[10], "............");
    strcpy(vgrid[11], "............");

    replace_one_of( vgrid, 12, 12, '%', '*', '%' );
    replace_one_of( vgrid, 12, 12, '9', '8', '9' );

    return MAP_NORTH;

}


static char minivault_28(char vgrid[81][81], FixedVector<int, 7>& mons_array)
{
    UNUSED( mons_array );

    strcpy(vgrid[0],  "............");
    strcpy(vgrid[1],  ".vvvvAvvvv..");
    strcpy(vgrid[2],  ".v.......v..");
    strcpy(vgrid[3],  ".v..999..v..");
    strcpy(vgrid[4],  ".v.9...9.v..");
    strcpy(vgrid[5],  ".A.9.I.9.A..");
    strcpy(vgrid[6],  ".v.9...9.v..");
    strcpy(vgrid[7],  ".v..999..v..");
    strcpy(vgrid[8],  ".v.......v..");
    strcpy(vgrid[9],  ".vvvvAvvvv..");
    strcpy(vgrid[10], "............");
    strcpy(vgrid[11], "............");

    replace_one_of( vgrid, 12, 12, 'A', '=', 'v' );

    return MAP_NORTH;

}


static char minivault_29(char vgrid[81][81], FixedVector<int, 7>& mons_array)
{

    strcpy(vgrid[0],  ".3......3...");
    strcpy(vgrid[1],  "...x.xx.x.2.");
    strcpy(vgrid[2],  ".xxx2xxxxx..");
    strcpy(vgrid[3],  ".xxxx42xxx2.");
    strcpy(vgrid[4],  ".2xx243432x3");
    strcpy(vgrid[5],  ".xx421424xx.");
    strcpy(vgrid[6],  "3xx423242x..");
    strcpy(vgrid[7],  ".x2x3243xxx.");
    strcpy(vgrid[8],  ".x2xx42422x.");
    strcpy(vgrid[9],  "..xxxxxxxx2.");
    strcpy(vgrid[10], "...x2xxxx3..");
    strcpy(vgrid[11], ".3.......33.");

    mons_array[0] = MONS_QUEEN_ANT;
    mons_array[1] = MONS_SOLDIER_ANT;
    mons_array[2] = MONS_GIANT_ANT;
    mons_array[3] = MONS_ANT_LARVA;

    return MAP_NORTH;

}


static char minivault_30(char vgrid[81][81], FixedVector<int, 7>& mons_array)
{     // lava pond
    UNUSED( mons_array );

    strcpy(vgrid[0],  "............");
    strcpy(vgrid[1],  "............");
    strcpy(vgrid[2],  "............");
    strcpy(vgrid[3],  "............");
    strcpy(vgrid[4],  "............");
    strcpy(vgrid[5],  ".....T......");
    strcpy(vgrid[6],  "............");
    strcpy(vgrid[7],  "............");
    strcpy(vgrid[8],  "............");
    strcpy(vgrid[9],  "............");
    strcpy(vgrid[10], "............");
    strcpy(vgrid[11], "............");

    return MAP_NORTH;

}


static char minivault_31(char vgrid[81][81], FixedVector<int, 7>& mons_array)
{     // lava pond
    UNUSED( mons_array );

    strcpy(vgrid[0],  "............");
    strcpy(vgrid[1],  "............");
    strcpy(vgrid[2],  "............");
    strcpy(vgrid[3],  "............");
    strcpy(vgrid[4],  "............");
    strcpy(vgrid[5],  ".....T......");
    strcpy(vgrid[6],  "............");
    strcpy(vgrid[7],  "............");
    strcpy(vgrid[8],  "............");
    strcpy(vgrid[9],  "............");
    strcpy(vgrid[10], "............");
    strcpy(vgrid[11], "............");

    return MAP_NORTH;

}


static char minivault_32(char vgrid[81][81], FixedVector<int, 7>& mons_array)
{     // lava pond
    UNUSED( mons_array );

    strcpy(vgrid[0],  "............");
    strcpy(vgrid[1],  "............");
    strcpy(vgrid[2],  "............");
    strcpy(vgrid[3],  "............");
    strcpy(vgrid[4],  "............");
    strcpy(vgrid[5],  ".....U......");
    strcpy(vgrid[6],  "............");
    strcpy(vgrid[7],  "............");
    strcpy(vgrid[8],  "............");
    strcpy(vgrid[9],  "............");
    strcpy(vgrid[10], "............");
    strcpy(vgrid[11], "............");

    return MAP_NORTH;

}


static char minivault_33(char vgrid[81][81], FixedVector<int, 7>& mons_array)
{     // lava pond
    UNUSED( mons_array );

    strcpy(vgrid[0],  "............");
    strcpy(vgrid[1],  "............");
    strcpy(vgrid[2],  "............");
    strcpy(vgrid[3],  "............");
    strcpy(vgrid[4],  "............");
    strcpy(vgrid[5],  ".....V......");
    strcpy(vgrid[6],  "............");
    strcpy(vgrid[7],  "............");
    strcpy(vgrid[8],  "............");
    strcpy(vgrid[9],  "............");
    strcpy(vgrid[10], "............");
    strcpy(vgrid[11], "............");

    return MAP_NORTH;

}

static char minivault_34(char vgrid[81][81], FixedVector<int, 7>& mons_array)
{
    //jmf: multi-god temple thing
    UNUSED( mons_array );

    strcpy(vgrid[0],  "............");
    strcpy(vgrid[1],  ".cccccccccc.");
    strcpy(vgrid[2],  ".cT......Tc.");
    strcpy(vgrid[3],  ".c..9..9..c.");
    strcpy(vgrid[4],  ".c..C..C..c.");
    strcpy(vgrid[5],  ".cT......Tc.");
    strcpy(vgrid[6],  ".cccc==cccc.");
    strcpy(vgrid[7],  ".ccc$$$$ccc.");
    strcpy(vgrid[8],  ".cc8....8cc.");
    strcpy(vgrid[9],  "..cc....cc..");
    strcpy(vgrid[10], "...cG..Gc...");
    strcpy(vgrid[11], "............");

    return MAP_NORTH;
}

static char minivault_35(char vgrid[81][81], FixedVector<int, 7>& mons_array)
{
    UNUSED( mons_array );
    //jmf: another multi-god temple thing

    strcpy(vgrid[0],  "............");
    strcpy(vgrid[1],  "..vvvvvvvv..");
    strcpy(vgrid[2],  ".vv......vv.");
    strcpy(vgrid[3],  ".v..c..c..v.");
    strcpy(vgrid[4],  ".v.Cc..cC.v.");
    strcpy(vgrid[5],  ".v..c..c..v.");
    strcpy(vgrid[6],  ".vT8c..c8Tv.");
    strcpy(vgrid[7],  ".vvvc==cvvv.");
    strcpy(vgrid[8],  "...Gc99cG...");
    strcpy(vgrid[9],  "...+*99*+...");
    strcpy(vgrid[10], "...GccccG...");
    strcpy(vgrid[11], "............");

    return MAP_NORTH;
}

static char demon_vault_1(char vgrid[81][81], FixedVector<int, 7>& mons_array)
{

    strcpy(vgrid[0],  "............");
    strcpy(vgrid[1],  ".xx3xx3x3xx.");
    strcpy(vgrid[2],  "..x.x..x.x..");
    strcpy(vgrid[3],  "..x.x..x.x..");
    strcpy(vgrid[4],  "..x.x..x.x..");
    strcpy(vgrid[5],  "..x2x32x3x..");
    strcpy(vgrid[6],  "..x3x13x2x..");
    strcpy(vgrid[7],  "..x.x..x.x..");
    strcpy(vgrid[8],  "..x.x..x.x..");
    strcpy(vgrid[9],  "..x.x..x.x..");
    strcpy(vgrid[10], ".xx3x3xx3xx.");
    strcpy(vgrid[11], "............");

    mons_array[0] = MONS_PANDEMONIUM_DEMON;
    mons_array[1] = rand_demon( DEMON_GREATER );
    mons_array[2] = rand_demon( DEMON_COMMON );
    mons_array[3] = RANDOM_MONSTER;
    mons_array[4] = RANDOM_MONSTER;
    mons_array[5] = RANDOM_MONSTER;

    return MAP_NORTH;
}


static char demon_vault_2(char vgrid[81][81], FixedVector<int, 7>& mons_array)
{

    strcpy(vgrid[0],  "............");
    strcpy(vgrid[1],  ".cccccccc3c.");
    strcpy(vgrid[2],  ".3~~~~~cc~c.");
    strcpy(vgrid[3],  ".cccccc4c~c.");
    strcpy(vgrid[4],  ".cc4c..cc~c.");
    strcpy(vgrid[5],  ".c~c.22.c~c.");
    strcpy(vgrid[6],  ".c~c.12.c~c.");
    strcpy(vgrid[7],  ".c~cc..c4cc.");
    strcpy(vgrid[8],  ".c~c4cccccc.");
    strcpy(vgrid[9],  ".c~cc~~~~~3.");
    strcpy(vgrid[10], ".c3cccccccc.");
    strcpy(vgrid[11], "............");

    mons_array[0] = MONS_PANDEMONIUM_DEMON;
    mons_array[1] = rand_demon( DEMON_GREATER );
    mons_array[2] = rand_demon( DEMON_COMMON );
    mons_array[3] = rand_demon( DEMON_COMMON );
    mons_array[4] = RANDOM_MONSTER;
    mons_array[5] = RANDOM_MONSTER;

    replace_with_func( vgrid, 12, 12, '~', trapped_area );

    return MAP_NORTH;
}


static char demon_vault_3(char vgrid[81][81], FixedVector<int, 7>& mons_array)
{

    strcpy(vgrid[0],  "............");
    strcpy(vgrid[1],  ".x.x.x3x.x..");
    strcpy(vgrid[2],  "..x.x3x3x.x.");
    strcpy(vgrid[3],  ".x.x.x2x.x..");
    strcpy(vgrid[4],  "..x3x2x2x3x.");
    strcpy(vgrid[5],  ".x3x2x1x2x3.");
    strcpy(vgrid[6],  "..x3x2x2x3x.");
    strcpy(vgrid[7],  ".x.x.x2x3x..");
    strcpy(vgrid[8],  "..x.x3x3x.x.");
    strcpy(vgrid[9],  ".x.x.x3x.x..");
    strcpy(vgrid[10], "..x.x.x.x.x.");
    strcpy(vgrid[11], "............");

    mons_array[0] = MONS_PANDEMONIUM_DEMON;
    mons_array[1] = rand_demon( DEMON_COMMON );
    mons_array[2] = rand_demon( DEMON_COMMON );

    return MAP_NORTH;
}


static char demon_vault_4(char vgrid[81][81], FixedVector<int, 7>& mons_array)
{
    //jmf: all 3s below were 1s -- may have been bug
    strcpy(vgrid[0],  "............");
    strcpy(vgrid[1],  ".cAcccccAc..");
    strcpy(vgrid[2],  ".A*=3=2=*A..");
    strcpy(vgrid[3],  ".ccccccc=c..");
    strcpy(vgrid[4],  ".c2=3=2c3c..");
    strcpy(vgrid[5],  ".c=ccc=c=c..");
    strcpy(vgrid[6],  ".c3c1=3=2c..");
    strcpy(vgrid[7],  ".c=ccccccc..");
    strcpy(vgrid[8],  ".A*=2=3=*A..");
    strcpy(vgrid[9],  ".cAcccccAc..");
    strcpy(vgrid[10], "............");
    strcpy(vgrid[11], "............");

    mons_array[0] = MONS_PANDEMONIUM_DEMON;
    mons_array[1] = rand_demon();
    mons_array[2] = rand_demon();

    replace_one_of( vgrid, 12, 12, 'A', '=', 'c' ); // outer door
    replace_one_of( vgrid, 12, 12, '*', '|', '*' );

    return MAP_NORTH;

}


static char demon_vault_5(char vgrid[81][81], FixedVector<int, 7>& mons_array)
{     // obviously possible to get stuck - too bad (should've come prepared)

    strcpy(vgrid[0],  "............");
    strcpy(vgrid[1],  "...xxxxxx...");
    strcpy(vgrid[2],  "..xx....xx..");
    strcpy(vgrid[3],  ".xx......xx.");
    strcpy(vgrid[4],  ".x..3232..x.");
    strcpy(vgrid[5],  ".x..2**3..x.");
    strcpy(vgrid[6],  ".x..3**2..x.");
    strcpy(vgrid[7],  ".x..2123..x.");
    strcpy(vgrid[8],  ".xx......xx.");
    strcpy(vgrid[9],  "..xx....xx..");
    strcpy(vgrid[10], "...xxxxxx...");
    strcpy(vgrid[11], "............");

    mons_array[0] = MONS_PANDEMONIUM_DEMON;
    mons_array[1] = rand_demon();
    mons_array[2] = rand_demon();

    replace_one_of( vgrid, 12, 12, '*', '|', '*' ); // inner door

    return MAP_NORTH;

}


static char demon_vault_6(char vgrid[81][81], FixedVector<int, 7>& mons_array)
{

    strcpy(vgrid[0],  "............");
    strcpy(vgrid[1],  "............");
    strcpy(vgrid[2],  "......2.....");
    strcpy(vgrid[3],  "............");
    strcpy(vgrid[4],  ".3..........");
    strcpy(vgrid[5],  "..........2.");
    strcpy(vgrid[6],  ".....1......");
    strcpy(vgrid[7],  "............");
    strcpy(vgrid[8],  "............");
    strcpy(vgrid[9],  ".2.......3..");
    strcpy(vgrid[10], "............");
    strcpy(vgrid[11], "............");

    mons_array[0] = MONS_PANDEMONIUM_DEMON;
    mons_array[1] = rand_demon();
    mons_array[2] = rand_demon();

    return MAP_NORTH;

}


static char demon_vault_7(char vgrid[81][81], FixedVector<int, 7>& mons_array)
{
    strcpy(vgrid[0],  "............");
    strcpy(vgrid[1],  ".ccc....ccc.");
    strcpy(vgrid[2],  ".c|ccAAcc|c.");
    strcpy(vgrid[3],  ".cc=3~~~=cc.");
    strcpy(vgrid[4],  "..c~cBBc4c..");
    strcpy(vgrid[5],  "..A~B12B~A..");
    strcpy(vgrid[6],  "..A~B23B~A..");
    strcpy(vgrid[7],  "..c4cBBc~c..");
    strcpy(vgrid[8],  ".cc=~~~3=cc.");
    strcpy(vgrid[9],  ".c|ccAAcc|c.");
    strcpy(vgrid[10], ".ccc....ccc.");
    strcpy(vgrid[11], "............");

    mons_array[0] = MONS_PANDEMONIUM_DEMON;
    mons_array[1] = rand_demon();
    mons_array[2] = rand_demon( DEMON_GREATER );
    mons_array[3] = rand_demon();

    replace_one_of( vgrid, 12, 12, 'A', '=', 'c' ); // outer door
    replace_one_of( vgrid, 12, 12, 'B', '=', 'c' ); // inner door

    replace_with_func( vgrid, 12, 12, '~', trapped_area );

    return MAP_NORTH;
}


static char demon_vault_8(char vgrid[81][81], FixedVector<int, 7>& mons_array)
{

    strcpy(vgrid[0],  "............");
    strcpy(vgrid[1],  "....ccccccc.");
    strcpy(vgrid[2],  "..ccc....1c.");
    strcpy(vgrid[3],  ".cc..2....c.");
    strcpy(vgrid[4],  ".c........c.");
    strcpy(vgrid[5],  ".cc.......c.");
    strcpy(vgrid[6],  "..cc33..2.c.");
    strcpy(vgrid[7],  "....33...cc.");
    strcpy(vgrid[8],  ".....c...c..");
    strcpy(vgrid[9],  "..F..cc.cc..");
    strcpy(vgrid[10], "......ccc...");
    strcpy(vgrid[11], "............");

    mons_array[0] = MONS_PANDEMONIUM_DEMON;
    mons_array[1] = rand_demon(DEMON_GREATER);
    mons_array[2] = rand_demon();

    return MAP_NORTH;
}


static char demon_vault_9( char vgrid[81][81], FixedVector<int, 7>& mons_array )
{
    strcpy(vgrid[0],  "............");
    strcpy(vgrid[1],  ".cccccccccc.");
    strcpy(vgrid[2],  ".c42=23=24c.");
    strcpy(vgrid[3],  ".c2cccccc2c.");
    strcpy(vgrid[4],  ".c=c1~~~c=c.");
    strcpy(vgrid[5],  ".c2c~~~~c3c.");
    strcpy(vgrid[6],  ".c3c~~~~c2c.");
    strcpy(vgrid[7],  ".c=c~~~~c=c.");
    strcpy(vgrid[8],  ".c2ccc=cc2A.");
    strcpy(vgrid[9],  ".c42=32c24c.");
    strcpy(vgrid[10], ".cccccccAcc.");
    strcpy(vgrid[11], "............");

    mons_array[0] = MONS_PANDEMONIUM_DEMON;
    mons_array[1] = rand_demon( DEMON_COMMON );
    mons_array[2] = rand_demon();
    mons_array[3] = rand_demon( DEMON_GREATER );

    replace_one_of( vgrid, 12, 12, 'A', '=', 'c' ); // outer door
    replace_with_func( vgrid, 12, 12, '~', trapped_treasure_area );

    return MAP_NORTH;
}

// Replaces one of the grids marked with the marker character, with
// the target character, the other marks get replaced with nontarget.
static bool replace_one_of( char vgrid[81][81], int mx, int my,
                            char marker, char target, char nontarget )
{
    int ox = -1, oy = -1;
    int count = 0;

    for (int x = 0; x < mx; x++)
    {
        for (int y = 0; y < my; y++)
        {
            if (vgrid[y][x] == marker)
            {
                count++;
                if (count == 1)
                {
                    ox = x;
                    oy = y;
                    vgrid[y][x] = target;
                }
                else if (one_chance_in( count ))
                {
                    vgrid[oy][ox] = nontarget;
                    ox = x;
                    oy = y;
                    vgrid[y][x] = target;
                }
                else
                {
                    vgrid[y][x] = nontarget;
                }
            }
        }
    }

    return (count > 0);
}

static void replace_with_func( char vgrid[81][81], int mx, int my,
                               char marker, char (*func)( char grid ) )
{
    for (int x = 0; x < mx; x++)
    {
        for (int y = 0; y < my; y++)
        {
            if (vgrid[y][x] == marker)
                vgrid[y][x] = func( vgrid[y][x] );
        }
    }
}

static char trapped_area( char grid )
{
    UNUSED( grid );

    return (one_chance_in(3) ? '^' : '.');
}

static char trapped_treasure_area( char grid )
{
    UNUSED( grid );

    if (one_chance_in(4))
        return (one_chance_in(4) ? '|' : '*');

    return (coinflip() ? '^' : '.');
}

static char plain_treasure_area( char grid )
{
    UNUSED( grid );

    return (one_chance_in(4) ? '$' : '%');
}

static char good_treasure_area( char grid )
{
    UNUSED( grid );

    int tmp = random2(1000);

    return ((tmp < 100) ? '|' :
            (tmp < 500) ? '*' :
            (tmp < 750) ? '%'
                        : '$');
}

static char great_treasure_area( char grid )
{
    UNUSED( grid );

    return (one_chance_in(4) ? '|' : '*');
}

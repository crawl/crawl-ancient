/*
 *  File:       beam.cc
 *  Summary:    Functions related to ranged attacks.
 *  Written by: Linley Henzell
 *
 *  Change History (most recent first):
 *
 *   <6>    07jan2001    GDL    complete rewrite.
 *   <5>    22July2000   GDL    allowed 'dummy' missiles from monsters
 *   <4>    11/14/99     cdl    evade beams with random40(ev) vice random2(ev)
 *                              all armour now protects against shrapnel
 *   <3>     6/ 2/99     DML    Added enums
 *   <2>     5/20/99     BWR    Added refreshs for curses
 *   <1>     -/--/--     LRH    Created
 */

#include "AppHdr.h"
#include "beam.h"

#include <stdlib.h>
#include <string.h>

#ifdef DOS
#include <dos.h>
#include <conio.h>
#endif

#include "externs.h"

#include "effects.h"
#include "fight.h"
#include "it_use2.h"
#include "itemname.h"
#include "items.h"
#include "misc.h"
#include "monstuff.h"
#include "mon-util.h"
#include "mstuff2.h"
#include "ouch.h"
#include "player.h"
#include "religion.h"
#include "skills.h"
#include "spells1.h"
#include "spells3.h"
#include "spells4.h"
#include "stuff.h"
#include "view.h"

//jmf: brent sez:
//  There's a reason curses is included after the *.h files in beam.cc.
//  There's a reason curses is included after the *.h files in beam.cc.
//  There's a reason curses is included after the *.h files in beam.cc.
//  There's a reason ...
#ifdef USE_CURSES
#include <curses.h>
#endif

#define BEAM_STOP       1000        // all beams stopped by subtracting this
                                    // from remaining range
#define MON_RESIST      0           // monster resisted
#define MON_UNAFFECTED  1           // monster unaffected
#define MON_AFFECTED    2           // monster was unaffected

static int spreadx[] = { 0, 0, 1, -1 };
static int spready[] = { -1, 1, 0, 0 };
static int opdir[]   = { 2, 1, 4, 3 };
static FixedArray < bool, 19, 19 > explode_map;

// helper functions (some of these, esp. affect(),  should probably
// be public):
static void sticky_flame_monster(int mn, bool source, int power);
static bool affectsWalls(struct bolt &beam);
static int affect(struct bolt &beam, int x, int y);
static bool isBouncy(struct bolt &beam);
static void beam_drop_object(struct bolt &beam, int inv_number, int x, int y);
static bool beam_term_on_target(struct bolt &beam);
static void beam_explodes(struct bolt &beam, int x, int y);
static int bounce(float &step1, float &step2, float w1, float w2, float &n1, float &n2,
    int l1, int l2, int &t1, int &t2, bool topBlocked, bool sideBlocked);
static bool fuzzyLine(float nx, float ny, int &tx, int &ty, int lx, int ly,
    float stepx, float stepy, bool roundX, bool roundY);
static int  affect_wall(struct bolt &beam, int x, int y);
static int  affect_place_clouds(struct bolt &beam, int x, int y);
static void affect_place_explosion_clouds(struct bolt &beam, int x, int y);
static int  affect_player(struct bolt &beam);
static void affect_items(struct bolt &beam, int x, int y);
static int  affect_monster(struct bolt &beam, struct monsters *mon);
static int  affect_monster_enchantment(struct bolt &beam, struct monsters *mon);
static int  range_used_on_hit(struct bolt &beam);
static void explosion1(struct bolt &pbolt);
static void explosion_map(struct bolt &beam, int x, int y,
    int count, int dir, int r);
static void explosion_cell(struct bolt &beam, int x, int y);


/*  NEW (GDL):
 *  Now handles all beamed/thrown items and spells,  tracers, and their effects.
 *  inv_number is used to determine what, if anything, was actually 'thrown',
 *  either from monster or player inventory.
 *
 *  if inv_number is -1,  there is no physical object being thrown that could
 *  land on the ground.
 */

/*
 * Beam pseudo code:
 *
 * 1. Calculate stepx and stepy - algorithms depend on finding a step axis
 *    which results in a line of rise 1 or less (ie 45 degrees or less)
 * 2. Calculate range.  Tracers always have max range, otherwise the beam
 *    will have somewhere between range and rangeMax
 * 3. Loop tracing out the line:
 *      3a. Check for walls and wall affecting beams
 *      3b. If no valid move is found, try a fuzzy move
 *      3c. If no valid move is yet found, try bouncing
 *      3d. If no valid move or bounce is found, break
 *      4. Check for beam termination on target
 *      5. Affect the cell which the beam just moved into -> affect()
 *      6. Decrease remaining range appropriately
 *      7. Check for early out due to aimedAtFeet
 *      8. Draw the beam
 * 9. Drop an object where the beam 'landed'
 *10. Beams explode where the beam 'landed'
 *11. If no message generated yet, send "nothing happens" (enchantments only)
 *
 */


void beam(struct bolt &pbolt, int inv_number)
{
    int dx, dy;             // total delta between source & target
    int lx, ly;             // last affected x,y
    float stepx, stepy;     // x,y increment
    float wx, wy;           // 'working' x,y
    bool beamTerminate;     // has beam been 'stopped' by something?
    float nx, ny;           // test(new) x,y - FP
    int   tx, ty;           // test(new) x,y - integer
    bool roundX, roundY;    // which to round?
    int rangeRemaining;
    bool fuzzyOK;           // fuzzification resulted in OK move
    bool sideBlocked, topBlocked;

//DEBUG
//    sprintf(info, "%s %s fired to %d, %d : t=%d c=%d f=%d (%d,%d) ",
//        (pbolt.isBeam)?"beam":"missile", (pbolt.isTracer)?"tracer":"",
//        pbolt.target_x, pbolt.target_y, pbolt.type, pbolt.colour,
//        pbolt.flavour, pbolt.hit, pbolt.damage );
//    mpr(info);

    // init
    pbolt.aimedAtFeet = false;
    pbolt.msgGenerated = false;
    pbolt.isExplosion = false;

    // first, calculate beam step
    dx = pbolt.target_x - pbolt.source_x;
    dy = pbolt.target_y - pbolt.source_y;

    // check for aim at feet
    if (dx == 0 && dy == 0)
    {
        pbolt.aimedAtFeet = true;
        stepx = 0.0;
        stepy = 0.0;
        roundY = false;
        roundX = false;
        tx = pbolt.source_x;
        ty = pbolt.source_y;
    }
    else
    {
        if (abs(dx) >= abs(dy))
        {
            stepx = (dx>0)?1.0:-1.0;
            stepy = (float)dy/(float)(abs(dx));
            roundY = true;
        }
        else
        {
            stepy = (dy>0)?1.0:-1.0;
            stepx = (float)dx/(float)(abs(dy));
            roundX = true;
        }
    }

    // give chance for beam to affect one cell even if aimedAtFeet.
    beamTerminate = false;
    // setup working coords
    lx = pbolt.source_x;
    wx = (float)lx;
    ly = pbolt.source_y;
    wy = (float)ly;
    // setup range
    rangeRemaining = pbolt.range;
    if (pbolt.rangeMax > pbolt.range)
    {
        if (pbolt.isTracer)
            rangeRemaining = pbolt.rangeMax;
        else
            rangeRemaining += random2((pbolt.rangeMax - pbolt.range) + 1);
    }

    // before we start drawing the beam, turn buffering off
#ifdef WIN32CONSOLE
    if (!pbolt.isTracer) setBuffering(false);
#endif

    // cannot use source_x, source_y, target_x, target_y during
    // step algorithm due to bouncing.

    // now, one step at a time, try to move towards target.
    while(!beamTerminate)
    {
        nx = wx + stepx;
        ny = wy + stepy;

        if (roundY)
        {
            tx = (int)nx;
            ty = (int)(ny + 0.5);
        }
        if (roundX)
        {
            ty = (int)ny;
            tx = (int)(nx + 0.5);
        }

        // check that tx, ty are valid.  If not,  set to last
        // x,y and break.
        if (tx < 0 || tx >= GXM || ty < 0 || ty >= GYM)
        {
            tx = lx;
            ty = ly;
            break;
        }

        // see if tx, ty is blocked by something
        if (grd[tx][ty] < MINMOVE)
        {
            // first, check to see if this beam affects walls.
            if (affectsWalls(pbolt))
            {
                // should we ever get a tracer with a wall-affecting
                // beam (possible I suppose),  we'll quit tracing now.
                if (!pbolt.isTracer)
                    rangeRemaining -= affect(pbolt, tx, ty);

                // if it's still a wall, quit.
                if (grd[tx][ty] < MINMOVE)
                    break;      // breaks from line tracing
            }
            else
            {
                // BEGIN fuzzy line algorithm
                fuzzyOK = fuzzyLine(nx,ny,tx,ty,lx,ly,stepx,stepy,roundX,roundY);
                if (!fuzzyOK)
                {
                    // BEGIN bounce case
                    if (!isBouncy(pbolt))
                    {
                        tx = lx;
                        ty = ly;
                        break;          // breaks from line tracing
                    }

                    sideBlocked = false;
                    topBlocked = false;
                    // BOUNCE -- guaranteed to return reasonable tx, ty.
                    // if it doesn't, we'll quit in the next if stmt anyway.
                    if (roundY)
                    {
                        if ( grd[lx + (int)stepx][ly] < MINMOVE)
                            sideBlocked = true;
                        if (dy != 0)
                            if ( grd[lx][ly + (stepy>0.0?1:-1)] < MINMOVE)
                                topBlocked = true;

                        rangeRemaining -= bounce(stepx, stepy, wx, wy, nx, ny,
                            lx, ly, tx, ty, topBlocked, sideBlocked);
                    }
                    else
                    {
                        if ( grd[lx][ly + (int)stepy] < MINMOVE)
                            sideBlocked = true;
                        if (dx != 0)
                            if ( grd[lx + (stepx>0.0?1:-1)][ly] < MINMOVE)
                                topBlocked = true;

                        rangeRemaining -= bounce(stepy, stepx, wy, wx, ny, nx,
                            ly, lx, ty, tx, topBlocked, sideBlocked);
                    }
                    // END bounce case - range check
                    if (rangeRemaining < 1)
                    {
                        tx = lx;
                        ty = ly;
                        break;
                    }
                }
            } // end else - beam doesn't affect walls
        } // endif - is tx, ty wall?

        // at this point, if grd[tx][ty] is still a wall, we
        // couldn't find any path: bouncy, fuzzy, or not - so break.
        if (grd[tx][ty] < MINMOVE)
        {
            tx = lx;
            ty = ly;
            break;
        }

        // check for "target termination"
        // occurs when beam can be targetted at empty
        // cell (e.g. a mage wants an explosion to happen
        // between two monsters)

        // in this case,  don't affect the cell - players
        // /monsters have no chance to dodge or block such
        // a beam,  and we want to avoid silly messages.
        if (tx == pbolt.target_x && ty == pbolt.target_y)
            beamTerminate = beam_term_on_target(pbolt);

        // affect the cell,  except in the special case noted
        // above -- affect() will early out if something gets
        // hit and the beam is type 'term on target'.
        if (!beamTerminate)
            rangeRemaining -= affect(pbolt, tx, ty);

        // always decrease range by 1
        rangeRemaining -= 1;

        // check for range termination
        if (rangeRemaining <= 0)
            beamTerminate = true;

        // special case - beam was aimed at feet
        if (pbolt.aimedAtFeet)
            beamTerminate = true;

        // actually draw the beam/missile/whatever.
        if (!pbolt.isTracer)
        {
            // we don't clean up the old position.
            // first, most people like seeing the full path,
            // and second, it is hard to do it right with
            // respect to killed monsters, cloud trails, etc.

            // draw new position
            int drawx = tx - you.x_pos + 18;
            int drawy = ty - you.y_pos + 9;
            // bounds check
            if (drawx > 8 && drawx < 26 && drawy > 0 && drawy < 18)
            {
                textcolor(pbolt.colour);
                gotoxy(drawx, drawy);
                putch(pbolt.type);
#ifdef USE_CURSES
                // If we don't refresh curses we won't
                // guarantee that the beam is visible
                refresh();
#endif
                delay(15);
            }

        }

        // set some stuff up for the next iteration
        lx = tx;
        ly = ty;

        wx = nx;
        wy = ny;

    } // end- while !beamTerminate

    // the beam has finished,  and terminated at tx, ty

    // leave an object, if applicable
    beam_drop_object(pbolt, inv_number, tx, ty);

    // check for explosion
    beam_explodes(pbolt, tx, ty);

    // canned msg for enchantments that affected no-one
    if (pbolt.beam_name[0] == '0' && pbolt.colour != BROWN)
        if (!pbolt.isTracer && !pbolt.msgGenerated &&!pbolt.obviousEffect)
            canned_msg(MSG_NOTHING_HAPPENS);

    // that's it!
}                               // end beam();


// returns damage taken by a monster from a "flavoured" (fire, ice, etc.)
// attack -- damage from clouds and branded weapons handled elsewhere.
int mons_adjust_flavoured(struct monsters *monster, struct bolt &pbolt,
                       int hurted, bool doFlavouredEffects)
{
    switch (pbolt.flavour)
    {
    case BEAM_FIRE:
        if (mons_res_fire(monster->type) > 0)
        {
            if (!pbolt.isTracer)
                simple_monster_message(monster, " appears unharmed.");
            hurted = 0;
        }
        else if (monster->inv[MSLOT_ARMOUR] != NON_ITEM
                 && mitm.special[monster->inv[MSLOT_ARMOUR]] % 30
                                                 == SPARM_FIRE_RESISTANCE)
        {
            if (!pbolt.isTracer)
                simple_monster_message(monster, " resists.");
            hurted /= 3;
        }

        if (mons_res_fire(monster->type) == -1
            && (monster->inv[MSLOT_ARMOUR] == NON_ITEM
                || mitm.special[monster->inv[MSLOT_ARMOUR]] % 30
                                                != SPARM_FIRE_RESISTANCE))
        {
            if (monster->type == MONS_ICE_BEAST
                || monster->type == MONS_SIMULACRUM_SMALL
                || monster->type == MONS_SIMULACRUM_LARGE)
            {
                if (!pbolt.isTracer)
                    simple_monster_message(monster, " melts!");
            }
            else
            {
                if (!pbolt.isTracer)
                    simple_monster_message(monster, " is burned terribly!");
            }
            hurted *= 15;
            hurted /= 10;
        }
        break;


    case BEAM_COLD:
        if (mons_res_cold(monster->type) > 0)
        {
            if (!pbolt.isTracer)
                simple_monster_message(monster, " appears unharmed.");
            hurted = 0;
        }
        else if (monster->inv[MSLOT_ARMOUR] != NON_ITEM
                 && mitm.special[monster->inv[MSLOT_ARMOUR]] % 30
                                                 == SPARM_COLD_RESISTANCE)
        {
            if (!pbolt.isTracer)
                simple_monster_message(monster, " resists.");
            hurted /= 3;
        }

        if (mons_res_cold(monster->type) == -1
            && (monster->inv[MSLOT_ARMOUR] == NON_ITEM
                || mitm.special[monster->inv[MSLOT_ARMOUR]] % 30
                                                != SPARM_COLD_RESISTANCE))
        {
            if (!pbolt.isTracer)
                simple_monster_message(monster, " is frozen!");
            hurted *= 15;
            hurted /= 10;
        }
        break;

    case BEAM_ELECTRICITY:
        if (mons_res_elec(monster->type) > 0)
        {
            if (!pbolt.isTracer)
                simple_monster_message(monster, " appears unharmed.");
            hurted = 0;
        }
        break;


    case BEAM_POISON:
        if (doFlavouredEffects && !pbolt.isTracer && !one_chance_in(3))
            poison_monster(monster, YOU_KILL(pbolt.thrower));

        if (mons_res_poison(monster->type) > 0)
        {
            if (doFlavouredEffects && !pbolt.isTracer)
                simple_monster_message(monster, " appears unharmed.");
            hurted = 0;
        }
        break;

    case BEAM_NEG:
        if (mons_holiness(monster->type) == MH_UNDEAD
            || mons_holiness(monster->type) == MH_DEMONIC)
        {
            if (!pbolt.isTracer)
                simple_monster_message(monster, " appears unharmed.");
            hurted = 0;
        }
        else
        {
            // early out for tracer/no side effects
            if (pbolt.isTracer || !doFlavouredEffects)
                return hurted;

            simple_monster_message(monster, " is drained.");

            if (one_chance_in(5))
                monster->hit_dice--;

            monster->max_hit_points -= 2 + random2(3);
            monster->hit_points -= 2 + random2(3);

            if (monster->hit_points >= monster->max_hit_points)
                monster->hit_points = monster->max_hit_points;

            if (monster->hit_dice < 1)
                monster->hit_points = 0;
        }                       // end else
        break;

    case BEAM_HOLY:             // flame of cleansing
        if (mons_holiness(monster->type) == MH_NATURAL
            || mons_holiness(monster->type) == MH_HOLY)
        {
            if (!pbolt.isTracer)
                simple_monster_message(monster, " appears unharmed.");
            hurted = 0;
        }
        break;

    case BEAM_ICE:
        /* ice - about 50% of damage is cold, other 50% is impact and
           can't be resisted (except by AC, of course) */
        if (mons_res_cold(monster->type) > 0)
        {
            if (!pbolt.isTracer)
                simple_monster_message(monster, " partially resists.");
            hurted /= 2;
        }
        else if (monster->inv[MSLOT_ARMOUR] != NON_ITEM
                 && mitm.special[monster->inv[MSLOT_ARMOUR]] % 30 ==
                 SPARM_COLD_RESISTANCE)
        {
            if (!pbolt.isTracer)
                simple_monster_message(monster, " partially resists.");
            hurted /= 2;
        }

        if (mons_res_cold(monster->type) == -1
            && (monster->inv[MSLOT_ARMOUR] == NON_ITEM
                || mitm.special[monster->inv[MSLOT_ARMOUR]] % 30
                                                != SPARM_COLD_RESISTANCE))
        {
            if (!pbolt.isTracer)
                simple_monster_message(monster, " is frozen!");
            hurted *= 13;
            hurted /= 10;
        }
        break;
    }                           /* end of switch */

    if (pbolt.flavour == BEAM_LAVA)    //jmf: lava != hellfire
    {
        if (mons_res_fire(monster->type) > 0
            || (monster->inv[MSLOT_ARMOUR] != NON_ITEM
                && mitm.special[monster->inv[MSLOT_ARMOUR]] % 30
                                                == SPARM_FIRE_RESISTANCE))
        {
            if (!pbolt.isTracer)
                simple_monster_message(monster, " partially resists.");
            hurted /= 2;
        }

        if (mons_res_fire(monster->type) == -1
            && (monster->inv[MSLOT_ARMOUR] == NON_ITEM
                || mitm.special[monster->inv[MSLOT_ARMOUR]] % 30
                                                != SPARM_FIRE_RESISTANCE))
        {
            if (monster->type == MONS_ICE_BEAST
                || monster->type == MONS_SIMULACRUM_SMALL
                || monster->type == MONS_SIMULACRUM_LARGE)
            {
                if (!pbolt.isTracer)
                    simple_monster_message(monster, " melts!");
            }
            else
            {
                if (!pbolt.isTracer)
                    simple_monster_message(monster, " is burned terribly!");
            }

            hurted *= 12;
            hurted /= 10;
        }
    }
    else if (stricmp(pbolt.beam_name, "hellfire") == 0)
    {
        if (mons_res_fire(monster->type) == 2)
        {
            if (!pbolt.isTracer)
                simple_monster_message(monster, " appears unharmed.");
            hurted = 0;
        }

        if (mons_res_fire(monster->type) == 1
            || (monster->inv[MSLOT_ARMOUR] != NON_ITEM
                && mitm.special[monster->inv[MSLOT_ARMOUR]] % 30
                                                == SPARM_FIRE_RESISTANCE))
        {
            if (!pbolt.isTracer)
                simple_monster_message(monster, " partially resists.");
            hurted /= 2;
        }

        if (mons_res_fire(monster->type) == -1
            && (monster->inv[MSLOT_ARMOUR] == NON_ITEM
                || mitm.special[monster->inv[MSLOT_ARMOUR]] % 30
                                                != SPARM_FIRE_RESISTANCE))
        {
            if (monster->type == MONS_ICE_BEAST
                || monster->type == MONS_SIMULACRUM_SMALL
                || monster->type == MONS_SIMULACRUM_LARGE)
            {
                if (!pbolt.isTracer)
                    simple_monster_message(monster, " melts!");
            }
            else
            {
                if (!pbolt.isTracer)
                    simple_monster_message(monster, " is burned terribly!");
            }

            hurted *= 12;       /* hellfire */
            hurted /= 10;
        }
    }

    return hurted;
}                               // end mons_adjust_flavoured()

// these return values seem "backward" to me {dlb}:

// Returns 0 if the monster made its save against hostile
// enchantments/some other magics.
bool check_mons_magres(struct monsters * monster, int pow)
{
    int mrs = mon_resist_mag(monster->type, monster->hit_dice);

    if (mrs == 5000)
        return false;

    // Evil, evil hack to make weak one hd monsters easier for first
    // level characters who have resistable 1st level spells. Six is
    // a very special value because mrs = hd * 2 * 3 for most monsters,
    // and the weak, low level monsters have been adjusted so that the
    // "3" is typically a 1.  There are some notable one hd monsters
    // that shouldn't fall under this, so we do < 6, instead of <= 6...
    // or checking monster->hit_dice.  The goal here is to make the
    // first level easier for these classes and give them a better
    // shot at getting to level two or three and spells that can help
    // them out (or building a level or two of their base skill so they
    // aren't resisted as often). -- bwr
    if (mrs < 6 && coinflip())
        return (false);

    if (pow > 40)               // nested if's rather than stacked 'em
    {                           // uglier than before but slightly
        pow = ((pow - 40) / 2) + 40;    // more efficient 16jan2000 {dlb}

        if (pow > 70)
        {
            pow = ((pow - 70) / 2) + 70;

            if (pow > 90)
            {
                pow = ((pow - 90) / 2) + 90;

                if (pow > 120)
                    pow = 120;
            }
        }
    }

    const int mrchance = (100 + mrs) - pow;
    const int mrch2 = random2(100) + random2(101);

#ifdef WIZARD
    strcpy(info, "Pow:");
    itoa(pow, st_prn, 10);
    strcat(info, st_prn);
    strcat(info, ", mrs: ");
    itoa(mrs, st_prn, 10);
    strcat(info, st_prn);
    strcat(info, ", mrchance:");
    itoa(mrchance, st_prn, 10);
    strcat(info, st_prn);
    strcat(info, ", mrch2:");
    itoa(mrch2, st_prn, 10);
    strcat(info, st_prn);
    mpr(info);
#endif

    return ((mrch2 < mrchance) ? false : true);
}                               // end check_mons_magres()

// Enchants all monsters in player's sight.
void mass_enchantment(int wh_enchant, int pow)
{
    int i = 0;                  // loop variable {dlb}
    int p;                      // loop variable {dlb}
    bool brek = false;
    bool msgGenerated = false;
    struct monsters *monster = 0;       // NULL {dlb}

    viewwindow(0, false);

    for (i = 0; i < MAX_MONSTERS; i++)
    {
        monster = &menv[i];

        if (monster->type == -1 || !mons_near(monster))
            continue;

        // assuming that the only mass charm is control undead:
        if (wh_enchant == ENCH_CHARM
            && (mons_holiness(monster->type) != MH_UNDEAD
                || monster->behavior == BEH_ENSLAVED))
        {
            continue;
        }

        if (wh_enchant != ENCH_CHARM
            || mons_holiness(monster->type) != MH_UNDEAD)
        {
            if (!check_mons_magres(monster, pow))
            {
                simple_monster_message(monster, " resists.");
                continue;
            }
        }

        if (mons_near(monster))
        {
            if (monster->enchantment1 == 1)
            {
                for (p = 0; p < 3; p++)
                {
                    if (monster->enchantment[p] == wh_enchant)
                        brek = true;
                }
            }

            for (p = 0; p < 3; p++)
            {
                if (brek)
                {
                    brek = false;
                    break;
                }

                if (monster->enchantment[p] == ENCH_NONE)
                {
                    monster->enchantment[p] = wh_enchant;
                    monster->enchantment1 = 1;

                    if (monster->enchantment[2] != ENCH_INVIS
                        || player_see_invis())
                    {
                        // turn message on
                        msgGenerated = true;
                        switch (wh_enchant)
                        {
                        case ENCH_FEAR:
                            simple_monster_message(monster,
                                                   " looks frightened.");
                            break;
                        case ENCH_CONFUSION:
                            simple_monster_message(monster,
                                                   " looks rather confused.");
                            break;
                        case ENCH_CHARM:
                            simple_monster_message(monster,
                                                   " submits to your will.");
                            monster->behavior = BEH_ENSLAVED;
                            break;
                        default:
                            // oops, I guess not!
                            msgGenerated = false;
                        }

                        break;  // I'm totally confused on this one {dlb}
                    }
                }
            }
        }                       // end "if mons_near(monster)"
    }                           // end "for i"

    if (!msgGenerated)
        canned_msg(MSG_NOTHING_HAPPENS);
}                               // end mass_enchantmenet()

/*
   Monster has probably failed save, now it gets enchanted somehow.

   returns MON_RESIST if monster is unaffected due to magic resist.
   returns MON_UNAFFECTED if monster is immune to enchantment
   returns MON_AFFECTED in all other cases (already enchanted, etc)
 */
int mons_ench_f2(struct monsters *monster, struct bolt &pbolt)
{
    bool is_near = mons_near(monster);  // single caluclation permissible {dlb}
    bool brek = false;
    char p;                     // loop variable

    switch (pbolt.colour)      /* put in magic resistance */
    {
    case BLACK:         /* 0 = slow monster */
        for (p = 0; p < 3; p++)
        {
            if (monster->enchantment[p] == ENCH_SLOW)
                return MON_AFFECTED;

            if (monster->enchantment[p] == ENCH_HASTE)
            {
                monster->enchantment[p] = ENCH_NONE;

                if (monster->speed >= 100)
                    monster->speed = 100 + ((monster->speed - 100) / 2);
                else
                    monster->speed /= 2;

                if (monster->enchantment[0] == ENCH_NONE
                    && monster->enchantment[1] == ENCH_NONE
                    && monster->enchantment[2] == ENCH_NONE)
                {
                    monster->enchantment1 = 0;
                }

                brek = true;
                break;
            }
        }                       /* end of for p */

        if (!brek)
        {
            for (p = 0; p < 3; p++)
            {
                if (monster->enchantment[p] == ENCH_NONE)
                    /* || p == 2) replaces 3rd enchantment if all full. */
                {
                    monster->enchantment[p] = ENCH_SLOW;

                    if (monster->speed >= 100)
                        monster->speed = 100 + ((monster->speed - 100) / 2);
                    else
                        monster->speed /= 2;

                    monster->enchantment1 = 1;
                    break;
                }
            }
        }

        // put in an exception for fungi, plants and other things you won't
        // notice slow down.
        if (simple_monster_message(monster, " seems to slow down."))
            pbolt.obviousEffect = true;
        return MON_AFFECTED;

    case BLUE:                  // 1 = haste
        for (p = 0; p < 3; p++)
        {
            if (monster->enchantment[p] == ENCH_HASTE)
                return MON_AFFECTED;

            if (monster->enchantment[p] == ENCH_SLOW)
            {
                monster->enchantment[p] = ENCH_NONE;

                if (monster->speed >= 100)
                    monster->speed = 100 + ((monster->speed - 100) * 2);
                else
                    monster->speed *= 2;

                if (monster->enchantment[0] == ENCH_NONE
                    && monster->enchantment[1] == ENCH_NONE
                    && monster->enchantment[2] == ENCH_NONE)
                {
                    monster->enchantment1 = 0;
                }

                brek = true;
                break;
            }
        }                       /* end of for p */

        if (!brek)
        {
            for (p = 0; p < 3; p++)
            {
                if (monster->enchantment[p] == ENCH_NONE)
                {
                    monster->enchantment[p] = ENCH_HASTE;

                    if (monster->speed >= 100)
                        monster->speed = 100 + ((monster->speed - 100) * 2);
                    else
                        monster->speed *= 2;

                    monster->enchantment1 = 1;
                    break;
                }
            }
        }

        if (p == 2 && monster->enchantment[p] != ENCH_NONE)
            return MON_AFFECTED;

        // put in an exception for fungi, plants and other things you won't
        // notice speed up.
        if (simple_monster_message(monster, " seems to speed up."))
            pbolt.obviousEffect = true;

        return MON_AFFECTED;


    case GREEN:         /* 2 = healing */
        if (heal_monster(monster, 5 + random2(15), false))
        {
            if (monster->hit_points == monster->max_hit_points)
            {
                if (simple_monster_message(monster,
                                        "'s wounds heal themselves!"))
                    pbolt.obviousEffect = true;
            }
            else
            {
                if (simple_monster_message(monster, " is healed somewhat."))
                    pbolt.obviousEffect = true;
            }
        }
        return MON_AFFECTED;

    case CYAN:                  /* 3 = paralysis */
        monster->speed_increment = 0;

        if (simple_monster_message(monster, " suddenly stops moving!"))
            pbolt.obviousEffect = true;

        if (grd[monster->x][monster->y] == DNGN_LAVA_X
            || grd[monster->x][monster->y] == DNGN_WATER_X)
        {
            if (mons_flies(monster->type) == 1)
            {
                // don't worry about invisibility - you should be able to
                // see if something has fallen into the lava
                if (is_near)
                {
                    strcpy(info, ptr_monam(monster, 0));
                    strcat(info, " falls into the ");
                    strcat(info, (grd[monster->x][monster->y] == DNGN_WATER_X)
                                ? "water" : "lava");
                    strcat(info, "!");
                    mpr(info);
                }

                switch (pbolt.thrower)
                {
                case KILL_YOU:
                case KILL_YOU_MISSILE:
                    monster_die(monster, KILL_YOU, pbolt.beam_source);
                    break;      /*  "    " */

                case KILL_MON:
                case KILL_MON_MISSILE:
                    monster_die(monster, KILL_MON_MISSILE,
                                pbolt.beam_source);
                    break;      /* dragon breath &c */
                }
            }
        }
        return MON_AFFECTED;

    case RED:                   /* 4 = confusion */
        for (p = 0; p < 3; p++)
        {
            if (monster->enchantment[p] == ENCH_CONFUSION)
                return MON_AFFECTED;
        }

        // replaces 3rd enchantment if all full:
        for (p = 0; p < 3; p++)
        {
            if (monster->enchantment[p] == ENCH_NONE || p == 2)
            {
                monster->enchantment[p] = ENCH_CONFUSION;
                monster->enchantment1 = 1;
                break;
            }
        }

        // put in an exception for fungi, plants and other things you won't
        // notice becoming confused.
        if (simple_monster_message(monster, " appears confused."))
            pbolt.obviousEffect = true;

        return MON_AFFECTED;

    case MAGENTA:               /* 5 = invisibility */
        if (monster->enchantment[2] == ENCH_INVIS || !is_near)
            return MON_AFFECTED;

        if (monster->enchantment[2] != ENCH_NONE)
        {
            if (is_near)
            {
                strcpy(info, ptr_monam(monster, 0));
                strcat(info, " flickers for a moment.");
                mpr(info);
                pbolt.obviousEffect = true;
            }
            return MON_AFFECTED;
        }

        if (is_near)
        {
            strcpy(info, ptr_monam(monster, 0));
            strcat(info, " flickers and vanishes!");
            mpr(info);
            pbolt.obviousEffect = true;
        }

        monster->enchantment[2] = ENCH_INVIS;
        monster->enchantment1 = 1;
        return MON_AFFECTED;

        /* 6 is used by digging
           7            teleport
           8            polymorph */

    case LIGHTBLUE:             /* 9 = charm */
        for (p = 0; p < 3; p++)
        {
            if (monster->enchantment[p] == ENCH_CHARM)
                return MON_AFFECTED;
        }

        for (p = 0; p < 4; p++)
        {
            if (p == 3)
                return MON_AFFECTED;

            if (monster->enchantment[p] == ENCH_NONE)
            {
                monster->enchantment[p] = ENCH_CHARM;
                monster->enchantment1 = 1;
                monster->behavior = BEH_ENSLAVED;
                break;
            }
        }

        // put in an exception for fungi, plants and other things you won't
        // notice becoming charmed.
        if (simple_monster_message(monster, " is charmed."))
            pbolt.obviousEffect = true;
        return MON_AFFECTED;
    default:
        break;
    }                           /* end of switch (beam_colour) */

    return MON_AFFECTED;
}                               // end mons_ench_f2()

// puts the poison value into a monster's enchantment variable.
void poison_monster(struct monsters *monster, bool fromPlayer)
{
    int p;
    bool brek = false;

    if (monster->type == -1)
        return;

    if (mons_res_poison(monster->type) > 0)
        return;

    if (monster->inv[MSLOT_ARMOUR] != NON_ITEM
        && mitm.special[monster->inv[MSLOT_ARMOUR]] % 30
                                            == SPARM_POISON_RESISTANCE)
    {
        return;
    }

    for (p = 0; p < 3; p++)
    {
        if (monster->enchantment[p] % 50 == 10)
            return;

        if (monster->enchantment[p] % 50 >= 7
            && monster->enchantment[p] % 50 < 10)
        {
            (monster->enchantment[p])++;
            brek = true;
            simple_monster_message(monster, " looks rather more sickly.");
            break;
        }
    }

    if (!brek)
    {
        for (p = 0; p < 3; p++)
        {
            if (monster->enchantment[p] == ENCH_NONE)
            {
                monster->enchantment[p] = ENCH_YOUR_POISON_I;
                monster->enchantment1 = 1;
                simple_monster_message(monster, " looks rather ill.");
                break;
            }
        }
    }

    if (!fromPlayer
        && monster->enchantment[p] <= ENCH_YOUR_POISON_IV
        && monster->enchantment[p] >= ENCH_YOUR_POISON_I)
    {
        monster->enchantment[p] += 50;
    }
    else
    {
        naughty(NAUGHTY_POISON, 5 + random2(3)); //jmf: TSO now hates poison
        done_good(GOOD_POISON, 5);      //jmf: had test god who liked poison
    }
}                               // end poison_monster()

// similar to poison_monster() -- makes the monster burn if hit by napalm
void sticky_flame_monster(int mn, bool source, int power)
{
    int long_last = 0;
    bool brek = false;
    int p;

    struct monsters *monster = &menv[mn];

    if (monster->type == -1)
        return;

    if (mons_res_fire(monster->type) > 0)
        return;

    if (monster->inv[MSLOT_ARMOUR] != NON_ITEM
        && mitm.special[monster->inv[MSLOT_ARMOUR]] % 30
                                        == SPARM_FIRE_RESISTANCE)
    {
        return;
    }

    long_last = 1 + (random2(power) / 2);

    if (long_last > 4)
        long_last = 4;

    for (p = 0; p < 3; p++)
    {
        if (monster->enchantment[p] == ENCH_YOUR_STICKY_FLAME_IV
            || monster->enchantment[p] == ENCH_STICKY_FLAME_IV)
        {
            // already covered in sticky flame - ouch!
            return;
        }
        if ((monster->enchantment[p] >= ENCH_YOUR_STICKY_FLAME_I
                && monster->enchantment[p] < ENCH_YOUR_STICKY_FLAME_IV)
            || (monster->enchantment[p] >= ENCH_STICKY_FLAME_I
                && monster->enchantment[p] < ENCH_STICKY_FLAME_IV))
        {
            monster->enchantment[p] += long_last;

            if (monster->enchantment[p] > ENCH_YOUR_STICKY_FLAME_IV
                && monster->enchantment[p] < ENCH_POISON_IV)
            {
                monster->enchantment[p] = ENCH_YOUR_STICKY_FLAME_IV;
            }

            if (monster->enchantment[p] > ENCH_STICKY_FLAME_IV)
                monster->enchantment[p] = ENCH_STICKY_FLAME_IV;

            brek = true;
            simple_monster_message(monster, " is covered in liquid fire!");
            break;
        }
    }                           /* end of for p */

    if (!brek)
    {
        for (p = 0; p < 3; p++)
        {
            if (monster->enchantment[p] == ENCH_NONE)
            {
                // should set long_last in a way that this referenced
                // ENCH_YOUR_STICKY_FLAME_I and not ENCH_CHARM {dlb}
                // ... try this -- bwr
                monster->enchantment[p] = ENCH_YOUR_STICKY_FLAME_I
                                            + long_last - 1;
                monster->enchantment1 = 1;
                simple_monster_message(monster,
                                       " is covered in liquid fire!");
                break;
            }
        }
    }

    if (source
        && monster->enchantment[p] <= ENCH_POISON_IV
        && monster->enchantment[p] >= ENCH_YOUR_STICKY_FLAME_I)
    {
        monster->enchantment[p] += 30;
    }
}                               // end sticky_flame_monster()

//   Places a cloud with the given stats. May delete old clouds to make way
//   if there are too many (MAX_CLOUDS == 30) on level. Will overwrite an old
//   cloud under some circumstances.
void place_cloud(unsigned char cl_type, unsigned char ctarget_x,
                 unsigned char ctarget_y, unsigned char cl_range)
{

    char c_deleted = (env.cloud_no >= MAX_CLOUDS) ? random2(MAX_CLOUDS) : 100;

    // more compact {dlb}
    char target_cgrid = env.cgrid[ctarget_x][ctarget_y];

    // that is, another cloud already there {dlb}
    if (target_cgrid != EMPTY_CLOUD)
    {
        if ((env.cloud_type[target_cgrid] >= CLOUD_GREY_SMOKE
                && env.cloud_type[target_cgrid] <= CLOUD_STEAM)
            || env.cloud_type[target_cgrid] == CLOUD_STINK
            || env.cloud_type[target_cgrid] == CLOUD_BLACK_SMOKE
            || env.cloud_decay[target_cgrid] <= 20)     //soon gone
        {
            env.cloud_type[env.cgrid[ctarget_x][ctarget_y]] = CLOUD_NONE;
            env.cloud_decay[env.cgrid[ctarget_x][ctarget_y]] = 0;
            env.cgrid[ctarget_x][ctarget_y] = EMPTY_CLOUD;
            env.cloud_no--;
        }
        else
            return;
    }

    for (unsigned char ci = 0; ci < MAX_CLOUDS; ci++)
    {
        if (ci == c_deleted)
        {
            env.cloud_type[ci] = CLOUD_NONE;
            env.cgrid[env.cloud_x[ci]][env.cloud_y[ci]] = EMPTY_CLOUD;
            env.cloud_decay[ci] = 0;
            env.cloud_no--;
        }

        if (env.cloud_type[ci] == CLOUD_NONE)   // ie is empty
        {
            env.cloud_type[ci] = cl_type;
            env.cloud_x[ci] = ctarget_x;
            env.cloud_y[ci] = ctarget_y;
            env.cloud_decay[ci] = cl_range * 10;
            env.cloud_no++;
            env.cgrid[env.cloud_x[ci]][env.cloud_y[ci]] = ci;
            break;
        }
    }
}                               // end place_cloud();

/*
 * Used by monsters in "planning" which spell to cast. Fires off a "tracer"
 * which tells the monster what it'll hit if it breathes/casts etc.
 *
 * The output from this tracer function is four variables in the beam struct:
 * fr_count, foe_count: a count of how many friends and foes will (probably)
 * be hit by this beam
 * fr_power, foe_power: a measure of how many 'friendly' hit dice it will
 *   affect,  and how many 'unfriendly' hit dice.
 *
 * note that beam properties must be set,  as the tracer will take them
 * into account,  as well as the monster's intelligence.
 *
 */
void fire_tracer(struct monsters *monster, struct bolt &pbolt)
{
    // don't fiddle with any input parameters other than tracer stuff!
    pbolt.isTracer = true;
    pbolt.source_x = monster->x;    // always safe to do.
    pbolt.source_y = monster->y;
    pbolt.beam_source = monster_index(monster);
    pbolt.canSeeInvis = (mons_see_invis(monster->type) != 0);
    pbolt.smartMonster = (mons_intel(monster->type) == I_HIGH ||
                          mons_intel(monster->type) == I_NORMAL);
    pbolt.isFriendly = (monster->behavior == BEH_ENSLAVED);

    // init tracer variables
    pbolt.foe_count = pbolt.fr_count = 0;
    pbolt.foe_power = pbolt.fr_power = 0;
    pbolt.foeRatio = 80;        // default - see mons_should_fire()

    // foe ratio for summon gtr. demons & undead -- they may be
    // summoned, but they're hostile and would love nothing better
    // than to nuke the player and his minions
    if (mons_holiness(monster->type) == MH_UNDEAD
        || mons_holiness(monster->type) == MH_DEMONIC)
        if (monster->enchantment[0] == ENCH_CHARM)
            pbolt.foeRatio = 25;

    // fire!
    beam(pbolt);

    // unset tracer flag (convenience)
    pbolt.isTracer = false;
}                               // end tracer_f()


/*
   When a mimic is hit by a ranged attack, it teleports away (the slow way)
   and changes its appearance - the appearance change is in monster_teleport
   in mstuff2.
 */
void mimic_alert(struct monsters *mimic)
{
    for (unsigned char p = 0; p < 3; p++)
    {
        if (mimic->enchantment[p] >= ENCH_TP_I
            && mimic->enchantment[p] <= ENCH_TP_IV)
        {
            return;
        }
    }

    monster_teleport(mimic, false);
}                               // end mimic_alert()

static bool isBouncy(struct bolt &beam)
{
    // at present, only non-enchantment eletrcical beams bounce.
    if (beam.beam_name[0] != '0' && beam.flavour == BEAM_ELECTRICITY)
        return true;
    return false;
}

static void beam_explodes(struct bolt &beam, int x, int y)
{
    int cloud_type;

    // this will be the last thing this beam does.. set target_x
    // and target_y to hold explosion co'ords
    beam.target_x = x;
    beam.target_y = y;

    // generic explosion
    if (beam.flavour == BEAM_EXPLOSION || beam.flavour == BEAM_HOLY)
    {
        explosion1(beam);
        return;
    }

    // cloud producer -- POISON BLAST
    if (strcmp(beam.beam_name, "blast of poison") == 0)
    {
        cloud_type = YOU_KILL(beam.thrower)?CLOUD_POISON:CLOUD_POISON_MON;
        big_cloud(cloud_type, x, y, 0, 9);
        return;
    }

    // cloud producer -- FOUL VAPOR (SWAMP DRAKE?)
    if (strcmp(beam.beam_name, "foul vapour") == 0)
    {
        cloud_type = YOU_KILL(beam.thrower)?CLOUD_STINK:CLOUD_STINK_MON;
        big_cloud(cloud_type, x, y, 0, 9);
        return;
    }

    // special cases - orbs & blasts of cold
    if (strcmp(beam.beam_name, "orb of electricity") == 0
        || strcmp(beam.beam_name, "metal orb") == 0
        || strcmp(beam.beam_name, "great blast of cold") == 0)
    {
        explosion1(beam);
        return;
    }

    // cloud producer only -- stinking cloud
    if (strcmp(beam.beam_name, "ball of vapour") == 0)
    {
        explosion1(beam);
        return;
    }
}

static bool beam_term_on_target(struct bolt &beam)
{

    // generic - all explosion-type beams can be targetted at empty space,
    // and will explode there.  This semantic also means that a creature
    // in the target cell will have no chance to dodge or block,  so we
    // DON'T affect() the cell if this function returns true!

    if (beam.flavour == BEAM_EXPLOSION
        || beam.flavour == BEAM_HOLY)
        return true;

    // POISON BLAST
    if (strcmp(beam.beam_name, "blast of poison") == 0)
        return true;

    // FOUL VAPOR (SWAMP DRAKE)
    if (strcmp(beam.beam_name, "foul vapour") == 0)
        return true;

    // STINKING CLOUD
    if (strcmp(beam.beam_name, "ball of vapour") == 0)
        return true;

    return false;
}

static void beam_drop_object(struct bolt &beam, int inv_number, int x, int y)
{
    // conditions: inv_number >= 0, beam is missile and not tracer.
    if (inv_number < 0 || beam.isTracer || beam.flavour != BEAM_MISSILE)
        return;

    if (YOU_KILL(beam.thrower))        // ie if you threw it.
    {
        if (grd[x][y] != DNGN_LAVA && grd[x][y] != DNGN_DEEP_WATER)
            if (you.inv_class[inv_number] != OBJ_MISSILES
                || !one_chance_in((you.inv_type[inv_number] == MI_STONE) ? 3 : 2))
                item_place(inv_number, x, y, 1);

        if (inv_number == you.equip[EQ_WEAPON])
        {
            you.equip[EQ_WEAPON] = -1;
            mpr("You are empty handed.");
        }
    }

    if (MON_KILL(beam.thrower) // monster threw it.
        && (grd[x][y] != DNGN_LAVA && grd[x][y] != DNGN_DEEP_WATER)
        && coinflip())
    {
        // 0 instead of 1 because the mitm.quantity is reduced by 1 when
        // it is thrown.
        if (mitm.quantity[inv_number] > 0)
        {
            if (igrd[x][y] != NON_ITEM)
            {
                if ((mitm.base_type[inv_number] == OBJ_MISSILES
                        || mitm.base_type[inv_number] == OBJ_FOOD
                        || mitm.base_type[inv_number] == OBJ_SCROLLS
                        || mitm.base_type[inv_number] == OBJ_POTIONS
                        || mitm.base_type[inv_number] == OBJ_UNKNOWN_II)
                    && mitm.base_type[inv_number] ==
                                mitm.base_type[igrd[x][y]]
                    && mitm.sub_type[inv_number] ==
                                mitm.sub_type[igrd[x][y]]
                    && mitm.pluses[inv_number] ==
                                mitm.pluses[igrd[x][y]]
                    && mitm.pluses2[inv_number] ==
                                mitm.pluses2[igrd[x][y]]
                    && mitm.special[inv_number] ==
                                mitm.special[igrd[x][y]])
                {
                    mitm.quantity[igrd[x][y]]++;
                    return;
                }
            }                   // end of if igrd != NON_ITEM

            int o = 0;

            for (o = 0; o < MAX_ITEMS; o++)
            {
                if (mitm.quantity[o] == 0)
                {
                    mitm.id[o] = mitm.id[inv_number];
                    mitm.base_type[o] = mitm.base_type[inv_number];
                    mitm.sub_type[o] = mitm.sub_type[inv_number];
                    mitm.pluses[o] = mitm.pluses[inv_number];
                    mitm.pluses2[o] = mitm.pluses2[inv_number];
                    mitm.special[o] = mitm.special[inv_number];
                    mitm.colour[o] = mitm.colour[inv_number];
                    mitm.quantity[o] = 1;
                    mitm.link[o] = NON_ITEM;
                    break;
                }
            }                   // end of o loop

            if (igrd[x][y] == NON_ITEM)
            {
                igrd[x][y] = o;
            }
            else
            {
                int hug = igrd[x][y];

                igrd[x][y] = o;
                mitm.link[o] = hug;
            }
        }                       // end of else
    }                           // if (thing_throw == 2) ...
}

// somewhat complicated BOUNCE function
// returns # of times beam bounces during routine (usually 1)
//
// step 1 is always the step value from the stepping direction.
#define B_HORZ      1
#define B_VERT      2
#define B_BOTH      3

static int bounce(float &step1, float &step2, float w1, float w2, float &n1, float &n2,
    int l1, int l2, int &t1, int &t2, bool topBlocked, bool sideBlocked)
{
    int bounceType = 0;
    int bounceCount = 1;
    float w2frac;

    if (topBlocked) bounceType = B_HORZ;
    if (sideBlocked) bounceType = B_VERT;
    if (topBlocked && sideBlocked)
    {
        // check for veritcal bounce only
        if ((int)(w2 + step2 - 0.5) == (int)(w2 - 0.5))
            bounceType = B_VERT;
        else
            bounceType = B_BOTH;
    }

    switch (bounceType)
    {
        case B_VERT:            // easiest
            n1 = w1;
            n2 = w2 + step2;
            step1 = -step1;
            t1 = (int)n1;
            t2 = (int)(n2 + 0.5);
            // check top
            if (t2 != (int)n2 && topBlocked)
                t2 = (int)n2;
            break;
        case B_HORZ:            // a little tricky
            if (step2 > 0.0)
                n2 = (float)(1 + 2*(int)w2) - (w2 + step2);
            else
                n2 = (float)(1 + 2*(int)(w2 - 0.5)) - (w2 + step2);
            n1 = w1 + step1;
            t1 = (int)n1;
            t2 = (int)(n2 + 0.5);
            step2 = -step2;
            break;
        case B_BOTH:
            // vertical:
            n1 = w1;
            t1 = l1;
            t2 = l2;
            // horizontal:
            if (step2 > 0.0)
                n2 = (float)(1 + 2*(int)w2) - (w2 + step2);
            else
                n2 = (float)(1 + 2*(int)(w2 - 0.5)) - (w2 + step2);
            // reverse both directions
            step1 =- step1;
            step2 =- step2;
            bounceCount = 2;
            break;
        default:
            bounceCount = 0;
            break;
    }

    return bounceCount;
}

static bool fuzzyLine(float nx, float ny, int &tx, int &ty, int lx, int ly,
    float stepx, float stepy, bool roundX, bool roundY)
{
    bool fuzzyOK = false;
    int fx, fy;                 // fuzzy x,y

    // BEGIN fuzzy line algorithm
    fx = tx;
    fy = ty;
    if (roundY)
    {
        // try up
        fy = (int)(ny + 0.999);
        // check for monotonic
        if (fy != ty && ((stepy>0.0 && fy >= ly)
            || (stepy<0.0 && fy <= ly)))
            fuzzyOK = true;
        // see if up try is blocked
        if (fuzzyOK && grd[tx][fy] < MINMOVE)
            fuzzyOK = false;

        // try down
        if (!fuzzyOK)
            fy = (int)ny;
        // check for monotonic
        if (fy != ty && ((stepy>0.0 && fy >= ly)
            || (stepy<0.0 && fy <= ly)))
            fuzzyOK = true;
        if (fuzzyOK && grd[tx][fy] < MINMOVE)
            fuzzyOK = false;
    }
    if (roundX)
    {
        // try up
        fx = (int)(nx + 0.999);
        // check for monotonic
        if (fx != tx && ((stepx>0.0 && fx >= lx)
            || (stepx<0.0 && fx <= lx)))
            fuzzyOK = true;
        // see if up try is blocked
        if (fuzzyOK && grd[fx][ty] < MINMOVE)
            fuzzyOK = false;

        // try down
        if (!fuzzyOK)
            fx = (int)nx;
        // check for monotonic
        if (fx != tx && ((stepx>0.0 && fx >= lx)
            || (stepx<0.0 && fx <= lx)))
            fuzzyOK = true;
        if (fuzzyOK && grd[fx][ty] < MINMOVE)
            fuzzyOK = false;
    }
    // END fuzzy line algorithm

    if (fuzzyOK)
    {
        tx = fx;
        ty = fy;
    }

    return fuzzyOK;
}

// affects a single cell.
// returns the amount of extra range 'used up' by this beam
// during the affectation.
//
// pseudo-code:
//
// 1. If wall, and wall affecting non-tracer, affect the wall.
//  1b.  If for some reason the wall-affect didn't make it into
//      a non-wall, return                      affect_wall()
// 2. for non-tracers, produce cloud effects    affect_place_clouds()
// 3. if cell holds player, affect player       affect_player()
// 4. if cell holds monster, affect monster     affect_monster()
// 5. return range used affectation.

static int affect(struct bolt &beam, int x, int y)
{
    // extra range used by hitting something
    int rangeUsed = 0;

    if (grd[x][y] < MINMOVE)
    {
        if (beam.isTracer)          // tracers always stop on walls.
            return BEAM_STOP;

        if (affectsWalls(beam))
        {
            rangeUsed += affect_wall(beam, x, y);
        }
        // if it's still a wall,  quit - we can't do anything else to
        // a wall.  Otherwise effects (like clouds, etc) are still possible.
        if (grd[x][y] < MINMOVE)
            return rangeUsed;
    }

    // grd[x][y] will NOT be a wall for the remainder of this function.

    // if not a tracer, place clouds
    if (!beam.isTracer)
        rangeUsed += affect_place_clouds(beam, x, y);

    // if player is at this location,  try to affect unless term_on_target
    if (x == you.x_pos && y == you.y_pos)
    {
        if (beam_term_on_target(beam) && !beam.isExplosion)
            return BEAM_STOP;
        rangeUsed += affect_player(beam);
    }

    // if there is a monster at this location,  affect it

    int mid = mgrd[x][y];
    if (mid != NON_MONSTER)
    {
        if (beam_term_on_target(beam) && !beam.isExplosion)
            return BEAM_STOP;

        struct monsters* monster = &menv[mid];
        rangeUsed += affect_monster(beam, monster);
    }

    return rangeUsed;
}

static bool affectsWalls(struct bolt &beam)
{
    // don't know of any explosion that affects walls.  But change it here
    // if there is.
    if (beam.isExplosion)
        return false;

    // digging
    if (beam.colour == BROWN && beam.beam_name[0] == '0')
        return true;

    // disintegration (or powerful disruption)
    if (beam.colour == WHITE && beam.flavour != BEAM_COLD
        && beam.hit >= 15)
        return true;

    // eye of devestation?
    if (beam.flavour == BEAM_NUKE)
        return true;

    return false;
}

// return amount of extra range used up by affectation of this wall.
static int  affect_wall(struct bolt &beam, int x, int y)
{
    int rangeUsed = 0;

    // DIGGING
    if (beam.colour == BROWN && beam.beam_name[0] == '0')
    {
        if (grd[x][y] == DNGN_STONE_WALL
            || grd[x][y] == DNGN_METAL_WALL
            || x <= 5 || x >= (GXM - 5)
            || y <= 5 || y >= (GYM - 5))
            return 0;

        if (grd[x][y] == DNGN_ROCK_WALL)
        {
            grd[x][y] = DNGN_FLOOR;

            if (!beam.msgGenerated)
            {
                if (!silenced(you.x_pos, you.y_pos))
                {
                    mpr("You hear a grinding noise.");
                    beam.obviousEffect = true;
                }
                beam.msgGenerated = true;
            }
        }
        return rangeUsed;
    }
    // END DIGGING EFFECT

    // NUKE / DISRUPT
    if ((beam.colour == WHITE && beam.flavour != BEAM_COLD && beam.hit >= 15)
        || beam.flavour == BEAM_NUKE)
    {
        int targ_grid = grd[ x ][ y ];

        if ((targ_grid == DNGN_ROCK_WALL || targ_grid == DNGN_WAX_WALL)
             && !(x <= 6 || y <= 6 || x >= (GXM - 6) || y >= (GYM - 6)))
        {
            grd[ x ][ y ] = DNGN_FLOOR;
            if (!silenced(you.x_pos, you.y_pos))
            {
                mpr("You hear a grinding noise.");
                beam.obviousEffect = true;
            }
        }

        if (targ_grid == DNGN_ORCISH_IDOL || (targ_grid >= DNGN_SILVER_STATUE
                && targ_grid <= DNGN_STATUE_39))
        {
            grd[ x ][ y ] = DNGN_FLOOR;

            if (!see_grid( x, y ))
                mpr("You hear a hideous screaming!");
            else
                mpr("The statue screams as its substance crumbles away!");
            beam.obviousEffect = 1;
        }

        return BEAM_STOP;
    }

    return rangeUsed;
}

static int affect_place_clouds(struct bolt &beam, int x, int y)
{
    int cloud_type;

    if (beam.isExplosion)
    {
        affect_place_explosion_clouds(beam,x,y);
        return 0;       // return value irrelevant for explosions
    }

    // check for CLOUD HITS
    if (env.cgrid[x][y] != EMPTY_CLOUD)     // hit a cloud
    {
        // polymorph randomly changes clouds in its path
        if (beam.beam_name[0] == '0' && beam.colour == DARKGREY)
            env.cloud_type[env.cgrid[x][y]] = 1 + random2(8);

        // now exit (all enchantments)
        if (beam.beam_name[0] == '0')
            return 0;

        int clouty = env.cgrid[x][y];

        // fire cancelling cold & vice versa
        if (((env.cloud_type[clouty] == CLOUD_COLD
                || env.cloud_type[clouty] == CLOUD_COLD_MON)
            && (beam.flavour == BEAM_FIRE
                || beam.flavour == BEAM_LAVA))
            || ((env.cloud_type[clouty] == CLOUD_FIRE
                || env.cloud_type[clouty] == CLOUD_FIRE_MON)
            && beam.flavour == BEAM_COLD))
        {
            if (!silenced(x, y)
                && !silenced(you.x_pos, you.y_pos))
            {
                mpr("You hear a sizzling sound!");
            }

            env.cloud_type[clouty] = CLOUD_NONE;
            env.cgrid[env.cloud_x[clouty]][env.cloud_y[clouty]] = EMPTY_CLOUD;
            env.cloud_decay[clouty] = 0;
            env.cloud_no--;

            return 5;
        }
    }

    // POISON BLAST
    if (strcmp(beam.beam_name, "blast of poison") == 0)
    {
        cloud_type = YOU_KILL(beam.thrower)?CLOUD_POISON:CLOUD_POISON_MON;

        place_cloud( cloud_type, x, y, random2(4) + 2 );
    }

    // FIRE/COLD over water/lava
    if ( (grd[x][y] == DNGN_LAVA && beam.flavour == BEAM_COLD)
        || ((grd[x][y] == DNGN_DEEP_WATER || grd[x][y] == DNGN_SHALLOW_WATER)
              && beam.flavour == BEAM_FIRE) )
    {
        place_cloud(CLOUD_STEAM, x, y, 2 + random2(5));
    }

    // ORB OF ENERGY
    if (strcmp(beam.beam_name, "orb of energy") == 0)
    {
        place_cloud(CLOUD_PURP_SMOKE, x, y, random2(5) + 1);
    }

    // GREAT BLAST OF COLD
    if (strcmp(beam.beam_name, "great blast of cold") == 0)
        place_cloud(CLOUD_COLD, x, y, random2(5) + 3);

    // BALL OF STEAM
    if (strcmp(beam.beam_name, "ball of steam") == 0)
    {
        place_cloud(CLOUD_STEAM, x, y, random2(5) + 1);
    }

    // STICKY FLAME
    if (strcmp(beam.beam_name, "sticky flame") == 0)
    {
        place_cloud(CLOUD_BLACK_SMOKE, x, y, random2(4) + 1);
    }

    // POISON GAS
    if (strcmp(beam.beam_name, "poison gas") == 0)
    {
        cloud_type = YOU_KILL(beam.thrower)?CLOUD_POISON:CLOUD_POISON_MON;
        place_cloud(CLOUD_POISON, x, y, random2(4) + 2);
    }

    return 0;
}

// following two functions used with explosions:

static void affect_place_explosion_clouds(struct bolt &beam, int x, int y)
{
    if (stricmp(beam.beam_name, "ice storm") == 0)
    {
        place_cloud(CLOUD_COLD, x, y, 2 + random2avg(5, 2));
    }

    if (stricmp(beam.beam_name, "stinking cloud") == 0)
    {
        int duration =  1 + random2(4) + random2((beam.ench_power / 50) + 1);
        place_cloud(CLOUD_STINK, x, y, duration);
    }
}

static void affect_items(struct bolt &beam, int x, int y)
{
    char objs_vulnerable = -1;

    switch (beam.flavour)
    {
    case BEAM_FIRE:
    case BEAM_LAVA:
        objs_vulnerable = OBJ_SCROLLS;
        break;
    case BEAM_COLD:
        objs_vulnerable = OBJ_POTIONS;
        break;
    case BEAM_SPORE:
        objs_vulnerable = OBJ_FOOD;
        break;
    }

    if (stricmp(beam.beam_name, "hellfire") == 0)
        objs_vulnerable = OBJ_SCROLLS;

    if (igrd[x][y] != NON_ITEM)
    {
        if (objs_vulnerable != -1 &&
            mitm.base_type[igrd[x][y]] == objs_vulnerable)
        {
            destroy_item( igrd[ x ][ y ] );

            if ( objs_vulnerable == OBJ_SCROLLS && see_grid(x,y))
            {
                mpr("You see a puff of smoke.");
            }
            if ( objs_vulnerable == OBJ_POTIONS && !silenced(x,y)
                && !silenced(you.x_pos, you.y_pos))
            {
                mpr("You hear glass shatter.");
            }
        }
    }
}

// return amount of extra range used up by affectation of the player
static int  affect_player(struct bolt &beam)
{
    int beamHit;

    // digging -- don't care.
    if (beam.beam_name[0] == '0' && beam.colour == BROWN)
        return 0;

    // check for tracer
    if (beam.isTracer)
    {
        // check can see player
        if (beam.canSeeInvis || !you.invis)
        {
            if (beam.isFriendly)
            {
                beam.fr_count += 1;
                beam.fr_power += you.experience_level;
            }
            else
            {
                beam.foe_count += 1;
                beam.foe_power += you.experience_level;
            }
        }
        return range_used_on_hit(beam);
    }

    // BEGIN real beam code
    beam.msgGenerated = true;

    // use beamHit,  NOT beam.hit,  for modification of tohit.. geez!
    beamHit = beam.hit;

    if (beam.beam_name[0] != '0')
    {
        // BEGIN BEAM/MISSILE

        // beams can only be dodged
        if (beam.isBeam && !beam.isExplosion)
        {
            if (player_light_armour()
                && !beam.aimedAtFeet && coinflip())
            {
                exercise(SK_DODGING, 1);
            }

            if (you.duration[DUR_REPEL_MISSILES]
                || you.mutation[MUT_REPULSION_FIELD] == 3)
            {
                beamHit -= random2(beamHit / 2);
            }

            if (you.duration[DUR_DEFLECT_MISSILES])
                beamHit = random2(beamHit / 3);

            if (beamHit < random2limit(player_evasion(), 40)
                                            + random2(you.dex) / 3 - 2
                && (!beam.aimedAtFeet))
            {
                strcpy(info, "The ");
                strcat(info, beam.beam_name);
                strcat(info, " misses you.");
                mpr(info);
                return 0;           // no extra used by miss!
            }
        }
        if (!beam.isBeam && !beam.isExplosion)
        {
            // non-beams can be blocked or dodged
            if (you.equip[EQ_SHIELD] != -1)
            {
                if (!beam.aimedAtFeet)
                {
                    if (coinflip())
                        exercise(SK_SHIELDS, 1);
                }
            }

            int hit = random2(beam.hit * 5 + 5 * you.shield_blocks);
            int block = random2(player_shield_class()) + (random2(you.dex) / 5) - 1;

            if (player_shield_class() > 0 && hit <= block)
            {
                you.shield_blocks++;
                strcpy(info, "You block the ");
                strcat(info, beam.beam_name);
                strcat(info, ".");
                mpr(info);
                return BEAM_STOP;
            }

            if (player_light_armour() && !beam.aimedAtFeet
                && coinflip())
                exercise(SK_DODGING, 1);

            if (you.duration[DUR_REPEL_MISSILES] != 0
                || you.mutation[MUT_REPULSION_FIELD] == 3)
            {
                beamHit = random2(beamHit);
            }

            int dodge = random2limit(player_evasion(), 40)
                                            + random2(you.dex) / 3 - 2;


            // miss message
            if (beamHit < dodge || you.duration[DUR_DEFLECT_MISSILES] != 0)
            {
                strcpy(info, "The ");
                strcat(info, beam.beam_name);
                strcat(info, " misses you.");
                return 0;
            }
        }
    }
    else
    {
        // BEGIN enchantment beam
        if (beam.colour != BLUE && beam.colour != MAGENTA
            && beam.colour != GREEN
            && ((beam.colour != LIGHTGREY
                    && beam.colour != LIGHTGREEN)
                || !beam.aimedAtFeet))
        {
            if (you_resist_magic(beam.ench_power))
            {
                canned_msg(MSG_YOU_RESIST);
                return range_used_on_hit(beam);
            }
        }

        // these colors are misapplied - see mons_ench_f2() {dlb}
        switch (beam.colour)
        {
        case BLACK:
            potion_effect(POT_SLOWING, beam.ench_power);
            beam.obviousEffect = true;
            break;     // slow

        case BLUE:
            potion_effect(POT_SPEED, beam.ench_power);
            beam.obviousEffect = true;
            break;     // haste

        case GREEN:
            potion_effect(POT_HEAL_WOUNDS, beam.ench_power);
            beam.obviousEffect = true;
            break;     // heal (heal wounds potion eff)

        case CYAN:
            potion_effect(POT_PARALYSIS, beam.ench_power);
            beam.obviousEffect = true;
            break;     // paralysis

        case RED:
            potion_effect(POT_CONFUSION, beam.ench_power);
            beam.obviousEffect = true;
            break;     // confusion

        case MAGENTA:
            potion_effect(POT_INVISIBILITY, beam.ench_power);
            beam.obviousEffect = true;
            break;     // invisibility

            // 6 is used by digging

        case LIGHTGREY:
            you_teleport();
            beam.obviousEffect = true;
            break;

        case DARKGREY:
            mpr("This is polymorph other only!");
            beam.obviousEffect = true;
            break;

        case LIGHTBLUE:
            potion_effect(POT_CONFUSION, beam.ench_power);
            beam.obviousEffect = true;
            break;     // enslavement - confusion?

        case LIGHTGREEN:
            if (you.level_type == LEVEL_ABYSS)
            {
                mpr("You feel trapped.");
                break;
            }
            mpr("You are cast into the Abyss!");
            more();
            banished(DNGN_ENTER_ABYSS);
            beam.obviousEffect = true;
            break;     // banishment to the abyss

        case LIGHTMAGENTA:      // pain
            if (you.is_undead || you.mutation[MUT_TORMENT_RESISTANCE])
            {
                mpr("You are unaffected.");
                break;
            }

            mpr("Pain shoots through your body!");
            strcpy(beam.beam_name, "spell");

            if (YOU_KILL(beam.thrower))
                ouch(1 + random2(beam.hit), 0, KILLED_BY_TARGETTING);
            else
            {
                ouch(1 + random2(beam.hit), beam.beam_source,
                     KILLED_BY_BEAM);
            }
            beam.obviousEffect = true;
            break;

        case WHITE:
            mpr("You are blasted!");
            strcpy(beam.beam_name, "spell");
            if (YOU_KILL(beam.thrower))
                ouch(random2(beam.hit), 0, KILLED_BY_TARGETTING);
            else
            {
                ouch(random2(beam.hit), beam.beam_source,
                     KILLED_BY_BEAM);
            }
            beam.obviousEffect = true;
            break;
        default:
            // _all_ enchantments should be enumerated here!
            mpr("Software bugs nibble your toes!");
            break;
        }               // end of switch (beam.colour)

        // regardless of affect, we need to know if this is a stopper
        // or not - it seems all of the above are.
        return range_used_on_hit(beam);

        // END enchantment beam
    }

    // THE BEAM IS NOW GUARANTEED TO BE A NON-ENCHANTMENT WHICH HIT

    strcpy( info, "The " );
    strcat( info, beam.beam_name );
    strcat( info, beam.isExplosion?" engulfs you!":" hits you!" );
    mpr(info);

    int hurted = 0;
    int burn_power = (beam.isExplosion)?5:((beam.isBeam)?3:2);

    if (beam.damage > 100)
    {
        hurted += random2(beam.damage - 100);
        hurted += random2(beam.damage - 100);
        hurted += random2(beam.damage - 100);
    }
    else
        hurted += random2(beam.damage);

    hurted -= random2(1 + player_AC());

    // shields don't help explosions
    if (!beam.isExplosion)
    {
        hurted -= random2(player_shield_class());
        you.shield_blocks++;
    }

    // shrapnel
    if (beam.flavour == BEAM_FRAG)
    {
        hurted -= random2(1 + player_AC());
        hurted -= random2(1 + player_AC());
    }

    // shield exercise for non-beams taken care of already..
    // no shield exercise for explosions (awww..)
    if (!beam.isExplosion && beam.isBeam && you.equip[EQ_SHIELD] != -1)
    {
        if (!beam.aimedAtFeet)
            exercise(SK_SHIELDS, (random2(3)) / 2);
    }

    if (you.equip[EQ_BODY_ARMOUR] != -1)
    {
        if (!player_light_armour() && one_chance_in(4)
            && random2(1000) <= mass(OBJ_ARMOUR,
                                     you.inv_type[you.equip[EQ_BODY_ARMOUR]]))
        {
            exercise(SK_ARMOUR, 1);
        }
    }

    if (hurted < 0)
        hurted = 0;

    hurted = check_your_resists(hurted, beam.flavour);

    // poisoning
    if (strstr(beam.beam_name, "poison") != NULL
        && beam.flavour != BEAM_POISON && !player_res_poison()
        && random2(hurted) - random2(player_AC()) > 0)
    {
        mpr("You are poisoned.");
        you.poison += 1 + random2(3);
    }

    // sticky flame
    if (strcmp(beam.beam_name, "sticky flame") == 0
        && (you.species != SP_MOTTLED_DRACONIAN
        || you.experience_level < 6))
    {
        if (you.equip[EQ_BODY_ARMOUR] == -1
            || you.inv_type[you.equip[EQ_BODY_ARMOUR]]
                            != ARM_MOTTLED_DRAGON_ARMOUR)
        {
            you.duration[DUR_LIQUID_FLAMES] +=
                                    random2avg(7, 3) + 1;
        }
    }

    // simple cases for scroll burns
    if (beam.flavour == BEAM_LAVA
        || stricmp(beam.beam_name, "hellfire") == 0)
        scrolls_burn(burn_power, OBJ_SCROLLS);

    // more complex (geez..)
    if (beam.flavour == BEAM_FIRE && strcmp(beam.beam_name, "ball of steam") != 0)
        scrolls_burn(burn_power, OBJ_SCROLLS);

    // potions exploding
    if (beam.flavour == BEAM_COLD)
        scrolls_burn(burn_power, OBJ_POTIONS);

    // spore pops
    if (beam.isExplosion && beam.flavour == BEAM_SPORE)
        scrolls_burn(2, OBJ_FOOD);

    if (YOU_KILL(beam.thrower))
        ouch(hurted, 0, KILLED_BY_TARGETTING);
    else
    {
        char killer = (beam.flavour == BEAM_SPORE)?KILLED_BY_SPORE:
            KILLED_BY_BEAM;
        ouch(hurted, beam.beam_source, killer);
    }

    return range_used_on_hit(beam);
}

// return amount of range used up by affectation of this monster
static int  affect_monster(struct bolt &beam, struct monsters *mon)
{
    int tid = mgrd[mon->x][mon->y];
    int hurt;
    int hurt_final;

    // digging -- don't care.
    if (beam.beam_name[0] == '0' && beam.colour == BROWN)
        return 0;

    // check for tracer
    if (beam.isTracer)
    {
        // check can see other monster
        if (!beam.canSeeInvis && menv[tid].enchantment[2] == ENCH_INVIS)
        {
            // can't see this monster, ignore it
            return 0;
        }
    }
    if (beam.beam_name[0] == '0')
    {
        if (beam.isTracer)
        {
            // enchant case -- enchantments always hit, so update target immed.
            if (beam.isFriendly ^ mon->behavior == BEH_ENSLAVED)
            {
                beam.foe_count += 1;
                beam.foe_power += mons_power(tid);
            }
            else
            {
                beam.fr_count += 1;
                beam.fr_power += mons_power(tid);
            }
            return range_used_on_hit(beam);
        }

        // BEGIN non-tracer enchantment

        // if a monster is hit with something and they can't see
        // you, they'll chase you anyway.  What!?
        if (you.invis && mon->behavior == BEH_SLEEP && !beam.canSeeInvis)
            mon->behavior = BEH_CHASING_I;

        // !@#*( affect_monster_enchantment() has side-effects on
        // the beam structure which screw up range_used_on_hit(),
        // so call it now and store.
        int rangeUsed = range_used_on_hit(beam);

        // now do enchantment affect
        int ench_result = affect_monster_enchantment(beam, mon);
        switch(ench_result)
        {
            case MON_RESIST:
                if (simple_monster_message(mon, " resists."))
                    beam.msgGenerated = true;
                break;
            case MON_UNAFFECTED:
                if (simple_monster_message(mon, " is unaffected."));
                    beam.msgGenerated = true;
                break;
            default:
                break;
        }
        return rangeUsed;

        // END non-tracer enchantment
    }


    // BEGIN non-enchantment (could still be tracer)

    if (monster_habitat(mon->type) != DNGN_FLOOR && mon->number == 1
        && !beam.aimedAtFeet)
        return 0;                   // missed me!

    // we need to know how much the monster _would_ be hurt by this,  before
    // we decide if it actually hits.

    hurt = 0;

    if (beam.damage > 100)
    {
        hurt += random2(beam.damage - 100);
        hurt += random2(beam.damage - 100);
        hurt += random2(beam.damage - 100);
    }
    else
    {
        hurt += random2(beam.damage);
    }

    hurt_final = hurt;

    if (beam.isTracer)
        hurt_final -= mon->armor_class / 2;
    else
        hurt_final -= random2(1 + mon->armor_class);

    if (beam.flavour == BEAM_FRAG)
    {
        hurt_final -= random2(1 + mon->armor_class);
        hurt_final -= random2(1 + mon->armor_class);
    }

    if (hurt_final < 1)
    {
        hurt_final = 0;
    }

    // check monster resists,  _without_ side effects (since the
    // beam/missile might yet miss!)
    hurt_final = mons_adjust_flavoured(mon, beam, hurt_final, false);

    // now,  we know how much this monster would (probably) be
    // hurt by this beam.
    if (beam.isTracer)
    {
        if (hurt_final != 0)
        {
            // monster could be hurt somewhat,  but only apply the
            // monster's power based on how badly it is affected.
            // For example,  if a fire giant (power 16) threw a
            // fireball at another fire giant,  and it only took
            // 1/3 damage,  then power of 5 would be applied to
            // foe_power or fr_power.
            if (beam.isFriendly ^ mon->behavior == BEH_ENSLAVED)
            {
                beam.foe_count += 1;
                beam.foe_power += hurt_final * mons_power(tid) / hurt;
            }
            else
            {
                beam.fr_count += 1;
                beam.fr_power += hurt_final * mons_power(tid) / hurt;
            }
        }
        // either way, we could hit this monster, so return range used
        return range_used_on_hit(beam);
    }
    // END non-enchantment (could still be tracer)

    // BEGIN real non-enchantment beam

    // explosions always 'hit'
    if (!beam.isExplosion && beam.hit < random2(mon->evasion))
    {
        // if the PLAYER cannot see the monster, don't tell them anything!
        if ((menv[tid].enchantment[2] != ENCH_INVIS || player_see_invis())
            && mons_near(mon))
        {
            strcpy(info, "The ");
            strcat(info, beam.beam_name);
            strcat(info, " misses ");
            strcat(info, ptr_monam(mon, 1));
            strcat(info, ".");
            mpr(info);
        }
        return 0;
    }

    // the beam hit.
    if (mons_near(mon))
    {
        strcpy(info, "The ");
        strcat(info, beam.beam_name);
        strcat(info, beam.isExplosion?" engulfs ":" hits ");
        if (menv[tid].enchantment[2] != ENCH_INVIS || player_see_invis())
            strcat(info, ptr_monam(mon, 1));
        else
            strcat(info, "something");
        strcat(info, ".");
        mpr(info);
    }
    else
    {
        // the player might hear something,
        // if _they_ fired a missile (not beam)
        if (!silenced(you.x_pos, you.y_pos) && beam.flavour == BEAM_MISSILE
                && YOU_KILL(beam.thrower))
        {
            strcpy(info, "The ");
            strcat(info, beam.beam_name);
            strcat(info, " hits something.");
            mpr(info);
        }
    }

    // note that hurt_final was calculated above, so we don't need it again.
    // just need to apply flavoured specials (since we called with
    // doFlavouredEffects = false above)
    mons_adjust_flavoured(mon, beam, hurt_final);

    // now hurt monster
    hurt_monster(mon, hurt_final);

    if (mon->behavior == BEH_SLEEP)
        mon->behavior = BEH_CHASING_I;

    int thrower = YOU_KILL(beam.thrower)?KILL_YOU_MISSILE:KILL_MON_MISSILE;

    if (thrower == KILL_YOU_MISSILE && mon->behavior == BEH_ENSLAVED
        && hurt_final > 0)
    {
        mon->behavior = BEH_CHASING_I;
        //naughty(NAUGHTY_STABBING, 5);
    }

    if (mon->hit_points < 1)
        monster_die(mon, thrower, beam.beam_source);
    else
    {
        if (thrower == KILL_YOU_MISSILE && mons_near(mon))
            print_wounds(mon);

        // sticky flame
        if (strcmp(beam.beam_name, "sticky flame") == 0)
            sticky_flame_monster(tid, false, hurt_final);

        /* looks for missiles which aren't poison but
           are poison*ed* */
        if (strstr(beam.beam_name, "poison") != NULL
            && beam.flavour != BEAM_POISON
            && random2(hurt_final) -
            random2(mon->armor_class) > 0)
        {
            poison_monster( mon, thrower );
        }

        if (mons_category(mon->type) == MC_MIMIC)
            mimic_alert(mon);
    }

    return range_used_on_hit(beam);
}

static int affect_monster_enchantment(struct bolt &beam, struct monsters *mon)
{
    if (beam.colour == LIGHTGREY) // teleportation
    {
        if (!check_mons_magres(mon, beam.ench_power)
            && !beam.aimedAtFeet)
            return MON_RESIST;

        if (simple_monster_message(mon, " looks slightly unstable."))
            beam.obviousEffect = true;

        monster_teleport(mon, false);
        return MON_AFFECTED;
    }

    if (beam.colour == DARKGREY)
    {
        if (!check_mons_magres(mon, beam.ench_power))
            return MON_RESIST;

        if (monster_polymorph(mon, RANDOM_MONSTER, 100))
            beam.obviousEffect = true;

        return MON_AFFECTED;
    }

    if (beam.colour == LIGHTGREEN)
    {
        if (!check_mons_magres(mon, beam.ench_power))
            return MON_RESIST;

        if (you.level_type == LEVEL_ABYSS)
        {
            simple_monster_message(mon, " wobbles for a moment.");
        }
        else
            monster_die(mon, KILL_RESET, beam.beam_source);

        beam.obviousEffect = true;
        return MON_AFFECTED;
    }

    if (beam.colour == LIGHTCYAN)
    {
        if (mons_holiness(mon->type) != MH_NATURAL
            || mon->type == MONS_PULSATING_LUMP)
            return MON_UNAFFECTED;

        if (!check_mons_magres(mon, beam.ench_power))
            return MON_RESIST;

        if (monster_polymorph(mon, MONS_PULSATING_LUMP, 100))
            beam.obviousEffect = true;

        return MON_AFFECTED;
    }

    if (beam.colour == YELLOW)
    {
        if (mons_holiness(mon->type) != MH_UNDEAD)
            return MON_UNAFFECTED;

        if (simple_monster_message(mon, " convulses!"))
            beam.obviousEffect = true;

        hurt_monster(mon, random2(beam.hit) + random2(beam.hit) +
                     random2(beam.hit));

        strcpy(beam.beam_name, "spell");
        goto deathCheck;
    }

    if (!check_mons_magres(mon, beam.ench_power)
        && beam.colour != BLUE && beam.colour != GREEN
        && beam.colour != MAGENTA)
        return MON_RESIST;

    if (beam.colour == LIGHTMAGENTA)      /* pain/agony */
    {
        if (mons_holiness(mon->type) == MH_UNDEAD
            || mons_holiness(mon->type) == MH_DEMONIC)
        {
            return MON_UNAFFECTED;
        }

        if (simple_monster_message(mon, " convulses in agony!"))
            beam.obviousEffect = true;

        if (strstr(beam.beam_name, "agony") != NULL)
        {
            mon->hit_points = mon->hit_points / 2;

            if (mon->hit_points < 1)
                mon->hit_points = 1;

            strcpy(beam.beam_name, "agony");
        }
        else
        {
            hurt_monster(mon, 1 + random2(beam.hit));
            strcpy(beam.beam_name, "pain");
        }

        goto deathCheck;
    }

    if (beam.colour == WHITE)     /* disrupt/disintegrate */
    {
        if (simple_monster_message(mon, " is blasted."))
            beam.obviousEffect = true;

        hurt_monster(mon, 1 + random2(beam.hit));

        strcpy(beam.beam_name, "spell");
        goto deathCheck;
    }

    if ((beam.colour == LIGHTRED && mons_holiness(mon->type) == MH_UNDEAD)
        || (beam.colour == 16 && mons_holiness(mon->type) == MH_DEMONIC))
    {
        if (simple_monster_message(mon, " is enslaved."))
            beam.obviousEffect = true;

        mon->behavior = BEH_ENSLAVED;
        return MON_AFFECTED;
    }

    if (beam.flavour == BEAM_SLEEP)
    {
        //jmf: FIXME: ought to do a better "affected by sleep" check
        if (mons_holiness(mon->type) == MH_NATURAL)
        {
            if (simple_monster_message(mon, " looks drowsy..."))
                beam.obviousEffect = true;
            mon->behavior = BEH_SLEEP;
            return MON_AFFECTED;
        }
    }


    if (beam.colour == LIGHTRED || beam.flavour == BEAM_SLEEP)
        return MON_UNAFFECTED;

    if (beam.flavour == BEAM_BACKLIGHT)
    {
        if (backlight_monsters(mon->x, mon->y,
                                beam.hit, 0))
            return MON_AFFECTED;
        return MON_UNAFFECTED;
    }

    // everything else?
    return mons_ench_f2(mon, beam);

deathCheck:

    int thrower = KILL_YOU_MISSILE;
    if (MON_KILL(beam.thrower))
        thrower = KILL_MON_MISSILE;

    if (mon->hit_points < 1)
        monster_die(mon, thrower, beam.beam_source);
    else
    {
        print_wounds(mon);

        if (mons_category(mon->type) == MC_MIMIC)
            mimic_alert(mon);
    }

    return MON_AFFECTED;
}


// extra range used on hit
static int  range_used_on_hit(struct bolt &beam)
{
    // non-beams can only affect one thing (player/monster)
    if (!beam.isBeam)
        return BEAM_STOP;

    // CHECK ENCHANTMENTS
    if (beam.beam_name[0] == '0')
    {
        switch(beam.colour)
        {
        case BLACK:
        case BLUE:
        case GREEN:
        case CYAN:
        case RED:
        case MAGENTA:
        case LIGHTGREY:
        case DARKGREY:
        case LIGHTBLUE:
        case LIGHTGREEN:
        case LIGHTMAGENTA:      // pain
        case WHITE:
        case LIGHTCYAN:
        case YELLOW:
        case LIGHTRED:
            return BEAM_STOP;
        default:
            break;
        }
        switch(beam.flavour)
        {
        case BEAM_SLEEP:
        case BEAM_BACKLIGHT:
            return BEAM_STOP;
        default:
            break;
        }
        return 0;
    }

    // hellfire stops for nobody!
    if (strcmp(beam.beam_name, "hellfire") == 0)
        return 0;

    // generic explosion
    if (beam.flavour == BEAM_EXPLOSION)
        return BEAM_STOP;

    // stinking cloud, plant spit
    if (beam.flavour == BEAM_ACID)
        return BEAM_STOP;

    // lava doesn't go far, but it goes through most stuff
    if (beam.flavour == BEAM_LAVA)
        return 1;

    // If it isn't lightning, reduce range by a lot
    if (beam.flavour != BEAM_ELECTRICITY)
        return random2(4) + 2;

    return 0;
}

/*
   Takes a bolt struct and refines it for use in the explosion function. Called
   from missile() and beam() in beam.cc. Explosions which do not follow from
   beams (eg scrolls of immolation) bypass this function.
 */
static void explosion1(struct bolt &pbolt)
{
    int ex_size = 1;
    // convenience
    int x = pbolt.target_x;
    int y = pbolt.target_y;
    char *seeMsg = NULL;
    char *hearMsg = NULL;

    // assume that the player can see/hear the explosion, or
    // gets burned by it anyway.  :)
    pbolt.msgGenerated = true;

    if (stricmp(pbolt.beam_name, "hellfire") == 0)
    {
        seeMsg = "The hellfire explodes!";
        hearMsg = "You hear a strangely unpleasant explosion.";

        pbolt.type = SYM_BURST;
        // Damage is now handled elsewhere.
        // pbolt.damage = 115;
        // hellfire - differ fr. DMNBM_HELLFIRE? [dlb]
        pbolt.flavour = BEAM_HELLFIRE;
    }

    if (stricmp(pbolt.beam_name, "golden flame") == 0)
    {
        seeMsg = "The flame explodes!";
        hearMsg = "You hear a strange explosion.";

        pbolt.type = SYM_BURST;
        pbolt.flavour = BEAM_HOLY;     // same as golden flame? [dlb]
    }

    if (stricmp(pbolt.beam_name, "fireball") == 0)
    {
        seeMsg = "The fireball explodes!";
        hearMsg = "You hear an explosion.";

        pbolt.type = SYM_BURST;
        // pbolt.damage += 100;   // it should already be set for this.
        pbolt.flavour = BEAM_FIRE;
        ex_size = 2;
    }

    if (stricmp(pbolt.beam_name, "orb of electricity") == 0)
    {
        seeMsg = "The orb of electricity explodes!";
        hearMsg = "You hear a clap of thunder!";

        pbolt.type = SYM_BURST;
        // pbolt.damage += 100;   // it should already be set for this.
        pbolt.flavour = BEAM_ELECTRICITY;
        pbolt.colour = LIGHTCYAN;
        ex_size = 2;
    }

    if (stricmp(pbolt.beam_name, "orb of energy") == 0)
    {
        seeMsg = "The orb of energy explodes.";
        hearMsg = "You hear an explosion.";
    }

    if (stricmp(pbolt.beam_name, "metal orb") == 0)
    {
        seeMsg = "The orb explodes into a blast of deadly shrapnel!";
        hearMsg = "You hear an explosion!";

        strcpy(pbolt.beam_name, "blast of shrapnel");
        pbolt.type = SYM_ZAP;
        // pbolt.damage += 100;   // it should already be set for this.
        pbolt.flavour = BEAM_FRAG;     // sets it from pure damage to shrapnel (which is absorbed extra by armour)
    }

    if (stricmp(pbolt.beam_name, "great blast of cold") == 0)
    {
        seeMsg = "The blast explodes into a great storm of ice!";
        hearMsg = "You hear a raging storm!";

        strcpy(pbolt.beam_name, "ice storm");
        pbolt.type = SYM_ZAP;
        // pbolt.damage += 100;   // it should already be set for this.
        pbolt.colour = WHITE;
        ex_size = 3;
    }

    if (stricmp(pbolt.beam_name, "ball of vapour") == 0)
    {
        seeMsg = "The ball expands into a vile cloud!";
        hearMsg = "You hear a gentle \'poof\'";
        strcpy(pbolt.beam_name, "stinking cloud");
    }

    if (seeMsg == NULL)
    {
        seeMsg = "The beam explodes into a cloud of software bugs!";
        hearMsg = "You hear the sound of one hand clapping!";
        return;
    }

    // check for see/hear/no msg
    if (see_grid(x,y) || (x == you.x_pos && y == you.y_pos))
        mpr(seeMsg);
    else
    {
        if (!(silenced(x,y) || silenced(you.x_pos, you.y_pos)))
            mpr(hearMsg);
        else
            pbolt.msgGenerated = false;
    }

    pbolt.ex_size = ex_size;
    explosion( pbolt );
}                               // end explosion1()


#define MAX_EXPLOSION_RADIUS 9

// explosion is considered to emanate from beam->target_x, target_y
// and has a radius equal to ex_size.  The explosion will respect
// boundaries like walls,  but go through/around statues/idols/etc.

// for each cell affected by the explosion, affect() is called.

void explosion(struct bolt &beam)
{
    int r = beam.ex_size;

    // beam is now an explosion;  set isExplosion.
    beam.isExplosion = true;

    // for now, we don't support explosions greater than 9 radius
    if (r > MAX_EXPLOSION_RADIUS)
        r = MAX_EXPLOSION_RADIUS;

    // make a noise
    noisy(10 + 5*r, beam.target_x, beam.target_y);

    // set map to false
    for (int i=0; i<19; i++)
        for (int j=0; j<19; j++)
            explode_map[i][j] = false;

    // discover affected cells - recursion is your friend!
    // this is done to model an explosion's behavior around
    // corners where a simple 'line of sight' isn't quite
    // enough.   This might be slow for really big explosions,
    // as the recursion runs approximately as R^2
    explosion_map(beam, 0, 0, 0, 0, r);

    // go through affected cells,  drawing effect and
    // calling affect() and affect_items() for each.
    // now, we get a bit fancy,  drawing all radius 0
    // effects, then radius 1, radius 2, etc.  It looks
    // a bit better that way.

    // turn buffering off
#ifdef WIN32CONSOLE
    if (!beam.isTracer) setBuffering(false);
#endif

    // do center
    explosion_cell(beam, 0, 0);

    // do the rest of it
    for(int rad = 1; rad <= r; rad ++)
    {
        // do sides
        for (int ay = 1 - rad; ay <= rad - 1; ay += 1)
        {
            if (explode_map[-rad+9][ay+9])
                explosion_cell(beam, -rad, ay);
            if (explode_map[rad+9][ay+9])
                explosion_cell(beam, rad, ay);
        }

        // do top & bottom
        for (int ax = -rad; ax <= rad; ax += 1)
        {
            if (explode_map[ax+9][-rad+9])
                explosion_cell(beam, ax, -rad);
            if (explode_map[ax+9][rad+9])
                explosion_cell(beam, ax, rad);
        }
    }

    // duplicate old behavior - pause after entire explosion
    // has been drawn.
    more();
}

static void explosion_cell(struct bolt &beam, int x, int y)
{
    int realx = beam.target_x + x;
    int realy = beam.target_y + y;

    int drawx = realx - you.x_pos + 18;
    int drawy = realy - you.y_pos + 9;

    if (see_grid(realx, realy) || (realx == you.x_pos && realy == you.y_pos))
    {
        // bounds check
        if (drawx > 8 && drawx < 26 && drawy > 0 && drawy < 18)
        {
            textcolor(beam.colour);
            gotoxy(drawx, drawy);
            putch('#');
    #ifdef USE_CURSES
            // If we don't refresh curses we won't
            // guarantee that the beam is visible
            refresh();
    #endif
            delay(10);
        }
    }

    affect(beam, realx, realy);

    affect_items(beam, realx, realy);
}

static void explosion_map(struct bolt &beam, int x, int y,
    int count, int dir, int r)
{
    // 1. check to see out of range
    if (x * x + y * y > r * r + r)
        return;

    // 2. check count
    if (count > 10*r)
        return;

    // 3. check to see if we're blocked by something
    //    specifically,  we're blocked by WALLS.  Not
    //    statues, idols, etc.
    int dngn_feat = grd[beam.target_x + x][beam.target_y + y];
    if (dngn_feat < DNGN_GREEN_CRYSTAL_WALL || dngn_feat == DNGN_WAX_WALL)
        return;

    // hmm, I think we're ok
    explode_map[x+9][y+9] = true;

    // now recurse in every direction except the one we
    // came from
    for(int i=0; i<4; i++)
    {
        if (i+1 != dir)
        {
            int cadd = 7;
            if (x * spreadx[i] < 0 || y * spready[i] < 0)
                cadd = 22;
            explosion_map(beam, x + spreadx[i], y + spready[i],
                count + cadd, opdir[i], r);
        }
    }
}

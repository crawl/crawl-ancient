/*
 *  File:       effects.cc
 *  Summary:    Misc stuff.
 *  Written by: Linley Henzell
 *
 *  Change History (most recent first):
 *
 *               <1>     -/--/--        LRH             Created
 */

#include "AppHdr.h"
#include "effects.h"

#include <string.h>
#include <stdio.h>

#include "externs.h"

#include "beam.h"
#include "direct.h"
#include "dungeon.h"
#include "fight.h"
#include "itemname.h"
#include "misc.h"
#include "monplace.h"
#include "monstuff.h"
#include "mon-util.h"
#include "mutation.h"
#include "newgame.h"
#include "ouch.h"
#include "player.h"
#include "skills2.h"
#include "spells3.h"
#include "spells4.h"
#include "spl-book.h"
#include "stuff.h"
#include "view.h"

void summon_butter(void);

// torment_monsters is called with power 0 because torment is
// UNRESISTABLE except for being undead or having torment
// resistance!   Even if we used maximum power of 1000,  high
// level monsters and characters would save too often.  (GDL)

void torment(char tx, char ty)
{
#ifdef USE_NEW_TORMENT_CODE
    apply_area_within_radius(torment_monsters, tx, ty, 0, 8, 0);
#else

    int dmi = 0;
    struct monsters *monster = 0;       // NULL {dlb}

    if (you.is_undead || you.mutation[MUT_TORMENT_RESISTANCE])
    {
        mpr("You feel a surge of unholy energy.");
    }
    else
    {
        mpr("Your body is wracked with pain!");
        dec_hp((you.hp / 2) - 1, false);
    }

    for (dmi = 0; dmi < MAX_MONSTERS; dmi++)
    {
        monster = &menv[dmi];

        if (monster->type == -1 || !mons_near(monster))
            continue;

        //int mres = random2(100);
        //int mres2 = mon_resist_mag(mons_class[dmi], mons_HD[dmi]);

        //if (mres < mres2)
        //  continue;

        if (mons_holiness(monster->type) > MH_NATURAL)
            continue;

        // maybe an exemption for undead?
        // maybe do something fun for magic circles?

        hurt_monster(monster, (monster->hit_points / 2) - 1);

        simple_monster_message(monster, " convulses!");
    }

#endif
}                               // end torment()

void banished(unsigned char gate_type)
{

    you_teleport2(false);
    // this is to ensure that you're standing on a suitable space (67)
    grd[you.x_pos][you.y_pos] = gate_type;
    down_stairs(true, you.your_level);  // heh heh

}                               // end banished()

bool forget_spell(void)
{
    unsigned char spc2;
    unsigned int safety = 0;    // to prevent infinite looping {dlb}

    if (you.spell_no < 1)
        return false;

    do
    {
        spc2 = random2(25);

        // begin safety check -- {dlb}
        safety++;

        if (safety > 1000)
            return false;
        // -- end safety check {dlb}

    }
    while (you.spells[spc2] == SPELL_NO_SPELL);

    you.spell_no--;
    you.spells[spc2] = SPELL_NO_SPELL;
    return true;
}                               // end forget_spell()

// use player::decrease_stats() instead iff:
// (a) player_sust_abil() should not factor in; and
// (b) there is no floor to the final stat values {dlb}
bool lose_stat(unsigned char which_stat, unsigned char stat_loss, bool force)
{
    bool statLowered = false;   // must initialize to false {dlb}
    char *ptr_stat = 0;         // NULL {dlb}
    char *ptr_redraw = 0;       // NULL {dlb}
    char newValue = 0;          // holds new value, for comparison to old {dlb}

    // begin outputing message: {dlb}
    strcpy(info, "You feel ");

    // set pointers to appropriate variables: {dlb}
    if (which_stat == STAT_RANDOM)
        which_stat = random2(NUM_STATS);

    switch (which_stat)
    {
    case STAT_STRENGTH:
        strcat(info, "weakened");
        ptr_stat = &you.strength;
        ptr_redraw = &you.redraw_strength;
        break;

    case STAT_DEXTERITY:
        strcat(info, "clumsy");
        ptr_stat = &you.dex;
        ptr_redraw = &you.redraw_dexterity;
        break;

    case STAT_INTELLIGENCE:
        strcat(info, "dopey");
        ptr_stat = &you.intel;
        ptr_redraw = &you.redraw_intelligence;
        break;
    }

    // scale modifier by player_sust_abil() - right-shift
    // permissible because stat_loss is unsigned: {dlb}
    if (!force)
        stat_loss >>= player_sust_abil();

    // newValue is current value less modifier: {dlb}
    newValue = *ptr_stat - stat_loss;

    // stats may be lowered to no less than three: {dlb}
    if (newValue < 3)
        newValue = 3;

    // conceivable that stat was already *at* three
    // or stat_loss zeroed by player_sust_abil(): {dlb}
    if (newValue != *ptr_stat)
    {
        *ptr_stat = newValue;
        *ptr_redraw = 1;

        // handle burden change, where appropriate: {dlb}
        if (ptr_stat == &you.strength)
            burden_change();

        statLowered = true;     // that is, stat changed {dlb}
    }

// a warning to player that s/he cut it close: {dlb}
    if (!statLowered)
        strcat(info, " for a moment");

// finish outputting message: {dlb}
    strcat(info, ".");
    mpr(info);

    return statLowered;
}                               // end lose_stat()

void direct_effect(struct bolt &pbolt)
{
    int damage_taken = 0;

    switch (pbolt.type)
    {
    case DMNBM_HELLFIRE:
        mpr("You are engulfed in a burst of hellfire!");
        pbolt.flavour = BEAM_LAVA;     // but it's hellfire anyway
        strcpy(pbolt.beam_name, "hellfire");   // for ouch
        scrolls_burn(4, OBJ_SCROLLS);
        damage_taken = 5 + random2avg(19, 2);
        damage_taken = check_your_resists(damage_taken, pbolt.flavour);
        break;

    case DMNBM_SMITING:
        mpr("Something smites you!");
        strcpy(pbolt.beam_name, "smiting");    // for ouch
        damage_taken = 7 + random2avg(11, 2);
        break;

    case DMNBM_BRAIN_FEED:
        // lose_stat() must come last {dlb}
        if (one_chance_in(3) && lose_stat(STAT_INTELLIGENCE, 1))
            mpr("Something feeds on your intellect!");
        else
            mpr("Something tries to feed on your intellect!");
        break;

    case DMNBM_MUTATION:
        mpr("Strange energies course through your body.");
        mutate(100);
        break;
    }

    // apply damage and handle death, where appropriate {dlb}
    if (damage_taken > 0)
        ouch(damage_taken, pbolt.beam_source, KILLED_BY_BEAM);

    return;
}                               // end direct_effect()

// monster-to-monster
void mons_direct_effect(struct bolt &pbolt, int i)
{
    // note the translation here - important {dlb}
    int o = menv[i].foe;
    struct monsters *monster = &menv[o];
    int damage_taken = 0;

    // annoy the target
    behavior_event(monster, ME_ANNOY, i);

    switch (pbolt.type)
    {
    case DMNBM_HELLFIRE:
        simple_monster_message(monster, " is engulfed in hellfire.");
        strcpy(pbolt.beam_name, "hellfire");
        pbolt.flavour = BEAM_LAVA;

        damage_taken = 5 + random2(10) + random2(5);
        damage_taken = mons_adjust_flavoured(monster, pbolt, damage_taken);
        break;

    case DMNBM_SMITING:
        simple_monster_message(monster, " is smitten.");
        strcpy(pbolt.beam_name, "smiting");
        pbolt.flavour = BEAM_MISSILE;

        damage_taken += 7 + random2avg(11, 2);
        break;

    case DMNBM_BRAIN_FEED:      // not implemented here (nor, probably, can be)
        break;

    case DMNBM_MUTATION:
        if (check_mons_magres(monster, pbolt.ench_power))
            simple_monster_message(monster, " resists.");
        else
            monster_polymorph(monster, RANDOM_MONSTER, 100);
        break;
    }

    // apply damage and handle death, where appropriate {dlb}
    if (damage_taken > 0)
    {
        hurt_monster(monster, damage_taken);

        if (monster->hit_points < 1)
            monster_die(monster, KILL_MON_MISSILE, i);
    }

    return;
}                               // end mons_direct_effect()

void random_uselessness(unsigned char ru, unsigned char sc_read_2)
{
    char wc[30];
    int temp_rand = 0;          // probability determination {dlb}

    switch (ru)
    {
    case 0:
        strcpy(info, "The dust glows a ");
        weird_colours(random2(256), wc);
        strcat(info, wc);
        strcat(info, " colour!");
        mpr(info);
        break;

    case 1:
        mpr("The scroll reassembles itself in your hand!");
        you.inv_quantity[sc_read_2]++;
        burden_change();
        break;

    case 2:
        if (you.equip[EQ_WEAPON] != -1)
        {
            in_name(you.equip[EQ_WEAPON], 4, str_pass);
            strcpy(info, str_pass);
            strcat(info, " glows ");
            weird_colours(random2(256), wc);
            strcat(info, wc);
            strcat(info, " for a moment.");
            mpr(info);
        }
        else
        {
            canned_msg(MSG_NOTHING_HAPPENS);
        }
        break;

    case 3:
        strcpy(info, "You hear the distant roaring of an enraged ");

        temp_rand = random2(8);

        strcat(info, (temp_rand == 0) ? "frog"          :
                     (temp_rand == 1) ? "pill bug"      :
                     (temp_rand == 2) ? "millipede"     :
                     (temp_rand == 3) ? "eggplant"      :
                     (temp_rand == 4) ? "albino dragon" :
                     (temp_rand == 5) ? "dragon"        :
                     (temp_rand == 6) ? "human"
                                      : "slug");

        strcat(info, "!");
        mpr(info);
        break;

    case 4:
        // josh declares mummies can't smell {dlb}
        if (you.species != SP_MUMMY)
        {
            strcpy(info, "You smell ");

            temp_rand = random2(8);

            strcat(info, (temp_rand == 0) ? "coffee."          :
                         (temp_rand == 1) ? "salt."            :
                         (temp_rand == 2) ? "burning hair!"    :
                         (temp_rand == 3) ? "baking bread."    :
                         (temp_rand == 4) ? "something weird." :
                         (temp_rand == 5) ? "wet wool."        :
                         (temp_rand == 6) ? "sulphur."
                                          : "fire and brimstone!");
            mpr(info);
        }
        break;

    case 5:
        mpr("You experience a momentary feeling of inescapable doom!");
        break;

    case 6:
        strcpy(info, "Your ");

        temp_rand = random2(3);

        strcat(info, (temp_rand == 0) ? "ears itch."   :
                     (temp_rand == 1) ? "brain hurts!"
                                      : "nose twitches suddenly!");
        mpr(info);
        break;

    case 7:
        mpr("You hear the tinkle of a tiny bell.");
        summon_butter();
        break;

    case 8:
        strcpy(info, "You hear ");

        temp_rand = random2(9);

        strcat(info, (temp_rand == 0) ? "snatches of song"                 :
                     (temp_rand == 1) ? "a voice call someone else's name" :
                     (temp_rand == 2) ? "a very strange noise"             :
                     (temp_rand == 3) ? "roaring flame"                    :
                     (temp_rand == 4) ? "a very strange noise indeed"      :
                     (temp_rand == 5) ? "the chiming of a distant gong"    :
                     (temp_rand == 6) ? "the bellowing of a yak"           :
                     (temp_rand == 7) ? "a crunching sound"
                                      : "the tinkle of an enormous bell");
        strcat(info, ".");
        mpr(info);
        break;
    }

    return;
}                               // end random_uselessness()

void acquirement(unsigned char force_class)
{

    int thing_created = 0;
    int iteration = 0;

    // Remember lava!
    unsigned char class_wanted = OBJ_RANDOM;
    unsigned char type_wanted = OBJ_RANDOM;

    unsigned char unique = 1;

    unsigned char acqc = 0;
    FixedVector < char, 50 > already_has;

    char best_spell = 99;
    char best_any = 99;
    unsigned char keyin;

    for (acqc = 0; acqc < 50; acqc++)
        already_has[acqc] = 0;

    if (force_class == OBJ_RANDOM)
    {
        mpr("This is a scroll of acquirement!");

      query:
        mpr("[a|A] Weapon   [b|B] Armour   [c|C] Jewellery   [d|D] Book");
        mpr("[e|E] Staff    [f|F] Food     [g|G] Miscellaneous ");
        //mpr("[r|R] - Just give me something good.");
        mpr("What kind of item would you like to acquire? ", MSGCH_PROMPT);

        keyin = get_ch();

        if (keyin == 'a' || keyin == 'A')
            class_wanted = OBJ_WEAPONS;
        else if (keyin == 'b' || keyin == 'B')
            class_wanted = OBJ_ARMOUR;
        else if (keyin == 'c' || keyin == 'C')
            class_wanted = OBJ_JEWELLERY;
        else if (keyin == 'd' || keyin == 'D')
            class_wanted = OBJ_BOOKS;
        else if (keyin == 'e' || keyin == 'E')
            class_wanted = OBJ_STAVES;
        else if (keyin == 'f' || keyin == 'F')
            class_wanted = OBJ_FOOD;
        else if (keyin == 'g' || keyin == 'G')
            class_wanted = OBJ_MISCELLANY;
    }
    else
        class_wanted = force_class;

    if (class_wanted > OBJ_ARMOUR)
    {
        for (acqc = 0; acqc < ENDOFPACK; acqc++)
        {
            if (you.inv_quantity[acqc] > 0
                    && you.inv_class[acqc] == class_wanted)
            {
                already_has[you.inv_type[acqc]] = 1;
            }
        }

        do
        {
                unsigned char i;
            switch (class_wanted)
            {
            case OBJ_JEWELLERY:
                // Try for a base type the player hasn't identified
                for (i = 0; i < 10; i++)
                {
                    type_wanted = random2(24);
                    if (one_chance_in(3))
                        type_wanted = AMU_RAGE + random2(10);

                    if (!get_id(OBJ_JEWELLERY, type_wanted))
                        break;
                }
                break;

            case OBJ_BOOKS:
                // remember, put rarer books higher in the list
                iteration = 1;
                type_wanted = 99;
                best_spell = best_skill( SK_SPELLCASTING, (NUM_SKILLS - 1),
                                         best_spell );

              which_book:
#ifdef WIZARD
                sprintf( info, "acquirement: iteration = %d, best_spell = %d",
                            iteration, best_spell );
                mpr(info);
#endif //jmf: debugging

                switch (best_spell)
                {
                default:
                case SK_SPELLCASTING:
                    if (you.skills[SK_SPELLCASTING] <= 3
                        && !you.had_item[BOOK_CANTRIPS])
                    {
                        // Handful of level one spells, very useful for the
                        // new spellcaster who's asking for a book -- bwr
                        type_wanted = BOOK_CANTRIPS;
                    }
                    else if (!you.had_item[BOOK_WIZARDRY])
                        type_wanted = BOOK_WIZARDRY;
                    else if (!you.had_item[BOOK_CONTROL])
                        type_wanted = BOOK_CONTROL;
                    else if (!you.had_item[BOOK_POWER])
                        type_wanted = BOOK_POWER;
                    break;

                case SK_POISON_MAGIC:
                    if (!you.had_item[BOOK_YOUNG_POISONERS])
                        type_wanted = BOOK_YOUNG_POISONERS;
                    else if (!you.had_item[BOOK_ENVENOMATIONS])
                        type_wanted = BOOK_ENVENOMATIONS;
                    break;

                case SK_EARTH_MAGIC:
                    if (!you.had_item[BOOK_GEOMANCY])
                        type_wanted = BOOK_GEOMANCY;
                    else if (!you.had_item[BOOK_EARTH])
                        type_wanted = BOOK_EARTH;
                    break;

                case SK_AIR_MAGIC:
                    // removed the book of clouds... all the other elements
                    // (and most other spell skills) only get two.
                    if (!you.had_item[BOOK_AIR])
                        type_wanted = BOOK_AIR;
                    else if (!you.had_item[BOOK_SKY])
                        type_wanted = BOOK_SKY;
                    break;

                case SK_ICE_MAGIC:
                    if (!you.had_item[BOOK_FROST])
                        type_wanted = BOOK_FROST;
                    else if (!you.had_item[BOOK_ICE])
                        type_wanted = BOOK_ICE;
                    break;

                case SK_FIRE_MAGIC:
                    if (!you.had_item[BOOK_FLAMES])
                        type_wanted = BOOK_FLAMES;
                    else if (!you.had_item[BOOK_FIRE])
                        type_wanted = BOOK_FIRE;
                    break;

                case SK_SUMMONINGS:
                    if (!you.had_item[BOOK_CALLINGS])
                        type_wanted = BOOK_CALLINGS;
                    else if (!you.had_item[BOOK_SUMMONINGS])
                        type_wanted = BOOK_SUMMONINGS;
                    else if (!you.had_item[BOOK_DEMONOLOGY])
                        type_wanted = BOOK_DEMONOLOGY;
                    break;

                case SK_ENCHANTMENTS:
                    best_any = best_skill(SK_FIGHTING, (NUM_SKILLS - 1), 99);

                    // So many enchantment books!  I really can't feel
                    // guilty at all for dividing out the fighting
                    // books and forcing the player to raise a fighting
                    // skill (or enchantments in the case of Crusaders)
                    // to get the remaining books... enchantments are
                    // much too good (most spells, lots of books here,
                    // id wand charges, gives magic resistance),
                    // something will eventually have to be done.  -- bwr
                    if (best_any >= SK_FIGHTING
                                && best_any <= SK_STAVES)
                    {
                        // Fighter mage's get the fighting enchantment books
                        if (!you.had_item[BOOK_WAR_CHANTS])
                            type_wanted = BOOK_WAR_CHANTS;
                        else if (!you.had_item[BOOK_TUKIMA])
                            type_wanted = BOOK_TUKIMA;
                    }
                    else if (!you.had_item[BOOK_CHARMS])
                        type_wanted = BOOK_CHARMS;
                    else if (!you.had_item[BOOK_HINDERANCE])
                        type_wanted = BOOK_HINDERANCE;
                    else if (!you.had_item[BOOK_ENCHANTMENTS])
                        type_wanted = BOOK_ENCHANTMENTS;
                    else if (!you.had_item[BOOK_CONTROL])
                        type_wanted = BOOK_CONTROL;

                    break;

                case SK_CONJURATIONS:
                    if (!you.had_item[BOOK_CONJURATIONS_I])
                        type_wanted = give_first_conjuration_book();
                    else if (!you.had_item[BOOK_TEMPESTS])
                        type_wanted = BOOK_TEMPESTS;
                    else if (!you.had_item[BOOK_ANNIHILATIONS])
                        type_wanted = BOOK_ANNIHILATIONS;
                    break;

                case SK_NECROMANCY:
                    if (!you.had_item[BOOK_NECROMANCY])
                        type_wanted = BOOK_NECROMANCY;
                    else if (!you.had_item[BOOK_DEATH])
                        type_wanted = BOOK_DEATH;
                    else if (!you.had_item[BOOK_UNLIFE])
                        type_wanted = BOOK_UNLIFE;
                    else if (!you.had_item[BOOK_NECRONOMICON])
                        type_wanted = BOOK_NECRONOMICON;
                    break;

                case SK_TRANSLOCATIONS:
                    if (!you.had_item[BOOK_SPATIAL_TRANSLOCATIONS])
                        type_wanted = BOOK_SPATIAL_TRANSLOCATIONS;
                    else if (!you.had_item[BOOK_WARP])
                        type_wanted = BOOK_WARP;
                    break;

                case SK_TRANSMIGRATION:
                    if (!you.had_item[BOOK_CHANGES])
                        type_wanted = BOOK_CHANGES;
                    else if (!you.had_item[BOOK_TRANSFIGURATIONS])
                        type_wanted = BOOK_TRANSFIGURATIONS;
                    else if (!you.had_item[BOOK_MUTATIONS])
                        type_wanted = BOOK_MUTATIONS;
                    break;

                case SK_DIVINATIONS:    //jmf: added 24mar2000
                    if (!you.had_item[BOOK_SURVEYANCES])
                        type_wanted = BOOK_SURVEYANCES;
                    else if (!you.had_item[BOOK_DIVINATIONS])
                        type_wanted = BOOK_DIVINATIONS;
                    break;
                }
/*
                if (type_wanted == 99 && glof == best_skill(SK_SPELLCASTING, (NUM_SKILLS - 1), 99))
*/
                if (type_wanted == 99 && iteration == 1)
                {
                    best_spell = best_skill( SK_SPELLCASTING, (NUM_SKILLS - 1),
                                             best_skill(SK_SPELLCASTING,
                                                        (NUM_SKILLS - 1), 99) );
                    iteration++;
                    goto which_book;
                }

                // if we don't have a book, try and get a new one.
                if (type_wanted == 99)
                {
                    do
                    {
                        type_wanted = random2(NUM_BOOKS);
                        if (one_chance_in(500))
                            break;
                    }
                    while (you.had_item[type_wanted]);
                }

                // if the book is invalid find any valid one.
                while (book_rarity(type_wanted) == 100
                                       || type_wanted == BOOK_DESTRUCTION
                                       || type_wanted == BOOK_MANUAL)
                {
                    type_wanted = random2(NUM_BOOKS);
                }
                break;

            case OBJ_STAVES:
                type_wanted = random2(18);
                if (class_wanted == OBJ_STAVES
                    && type_wanted > STAFF_SUMMONING_I && !one_chance_in(5))
                {
                    type_wanted = random2(10);
                }
                break;

            case OBJ_MISCELLANY:
                type_wanted = random2(6);
                break;

// BCR - You can now acquire food!
            case OBJ_FOOD:
                unique = 4 + random2(7);
                if (you.species == SP_GHOUL)
                {
                    if (one_chance_in(3))
                        type_wanted = FOOD_ROYAL_JELLY;
                    else
                    {
                        type_wanted = FOOD_CHUNK;
                        unique += 10;
                    }
                }
                else
                {
                    type_wanted = FOOD_ROYAL_JELLY;
                }
                break;

            default:
                mesclr();
                goto query;
            }
        }
        while (already_has[type_wanted] == 1 && !one_chance_in(200));

    }
    else if (class_wanted == OBJ_WEAPONS)
        type_wanted = OBJ_RANDOM;    // 1 + random2(18); // weapons - no clubs
    else
        type_wanted = OBJ_RANDOM;    // always get random armour

    if (grd[you.x_pos][you.y_pos] == DNGN_LAVA
                        || grd[you.x_pos][you.y_pos] == DNGN_DEEP_WATER)
    {
        mpr("You hear a splash.");      // how sad (and stupid)
    }
    else
    {
        // BCR - unique is now used for food quantity.
        thing_created = items(unique, class_wanted, type_wanted, 1, 351, 250);

        if (type_wanted == OBJ_RANDOM
            && (class_wanted == OBJ_WEAPONS
                || class_wanted == OBJ_ARMOUR
                || class_wanted == OBJ_JEWELLERY))
        {
            if (mitm.pluses[thing_created] > 130)
                mitm.pluses[thing_created] -= 100;
        }

        canned_msg(MSG_SOMETHING_APPEARS);

        // link to top
        mitm.link[thing_created] = igrd[you.x_pos][you.y_pos];
        igrd[you.x_pos][you.y_pos] = thing_created;
    }
    return;
}                               // end acquirement()

bool recharge_wand(void)
{
    if (you.equip[EQ_WEAPON] == -1
            || you.inv_class[you.equip[EQ_WEAPON]] != OBJ_WANDS)
    {
        return false;
    }

    unsigned char charge_gain = 0;

    switch (you.inv_type[you.equip[EQ_WEAPON]])
    {
    case WAND_FIREBALL:
    case WAND_LIGHTNING:
    case WAND_DRAINING:
    case WAND_HEALING:
        charge_gain = 4;
        break;
    case WAND_FIRE:
    case WAND_COLD:
        charge_gain = 5;
        break;
    default:
        charge_gain = 8;
        break;
    }

    in_name(you.equip[EQ_WEAPON], 4, str_pass);

    strcpy(info, str_pass);
    strcat(info, " glows for a moment.");
    mpr(info);

    you.inv_plus[you.equip[EQ_WEAPON]] +=
                            1 + random2avg( ((charge_gain - 1) * 3) + 1, 3 );

    if (you.inv_plus[you.equip[EQ_WEAPON]] > charge_gain * 3)
        you.inv_plus[you.equip[EQ_WEAPON]] = charge_gain * 3;

    return true;
}                               // end recharge_wand()

void yell(void)
{
    bool targ_prev = false;
    int mons_targd = 0;
    struct dist targ;

    if (silenced(you.x_pos, you.y_pos))
    {
        mpr("You are unable to make a sound!");
        return;
    }

    mpr("What do you say?", MSGCH_PROMPT);
    mpr(" ! - Yell");
    mpr(" a - Order allies to attack a monster");

    if (!(you.prev_targ == MHITNOT || you.prev_targ == MHITYOU))
    {
        struct monsters *target = &menv[you.prev_targ];

        if (mons_near(target)
            && (!mons_has_ench(target,ENCH_INVIS) || player_see_invis()))
        {
            mpr(" p - Order allies to attack your previous target");
            targ_prev = true;
        }
    }

    strcpy(info, " Anything else - Stay silent");

    if (one_chance_in(10))
        strcat(info, " (and be thought a fool)");

    mpr(info);

    unsigned char keyn = get_ch();

    switch (keyn)
    {
    case '!':
        mpr("You yell for attention!");
        you.turn_is_over = 1;
        noisy(12, you.x_pos, you.y_pos);
        return;

    case 'a':
        mpr("Gang up on whom?", MSGCH_PROMPT);
        direction(targ, DIR_TARGET);

        if (targ.isCancel)
        {
            canned_msg(MSG_OK);
            return;
        }

        if (!targ.isValid || mgrd[targ.tx][targ.ty] == NON_MONSTER)
        {
            mpr("Yeah, whatever.");
            return;
        }

        mons_targd = mgrd[targ.tx][targ.ty];
        break;

    case 'p':
        if (targ_prev)
        {
            mons_targd = you.prev_targ;
            break;
        }
        /* fall through... */
    default:
        mpr("Okely-dokely.");
        return;
    }

    you.pet_target = mons_targd;

    noisy(10, you.x_pos, you.y_pos);
    mpr("Attack!");
}                               // end yell()

// produce caterpillars under rare circumstances?
// why did jmf change it to BEH_ENSLAVED ??? {dlb}
void summon_butter(void)
{
    for (int scount = 0; scount < 8; scount++)
    {
        create_monster( MONS_BUTTERFLY, ENCH_ABJ_III, BEH_FRIENDLY,
            you.x_pos, you.y_pos, MHITNOT, 250 );
    }
}                               // end summon_butter()

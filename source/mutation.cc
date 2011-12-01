/*
 *  File:       mutation.cc
 *  Summary:    Functions for handling player mutations.
 *  Written by: Linley Henzell
 *
 *  Change History (most recent first):
 *
 *      <5>      7/29/00        JDJ             Made give_cosmetic_mutation static
 *      <4>      9/25/99        CDL             linuxlib -> liblinux
 *      <3>      9/21/99        LRH             Added many new scales
 *      <2>      5/20/99        BWR             Fixed it so demonspwan should
 *                                              always get a mutation, 3 level
 *                                              perma_mutations now work.
 *      <1>      -/--/--        LRH             Created
 */

#include "AppHdr.h"
#include "mutation.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#ifdef DOS
#include <conio.h>
#endif

#ifdef LINUX
#include "liblinux.h"
#endif

#include "globals.h"
#include "externs.h"

#include "defines.h"
#include "effects.h"
#include "macro.h"
#include "ouch.h"
#include "player.h"
#include "skills2.h"
#include "stuff.h"
#include "transfor.h"
#include "view.h"

struct mutation_def
{
    int                  type;
    int                  rarity;            // rarity of getting 1st level
    int                  inc_rarity;        // rarity of getting more levels
    int                  max_levels;        // maximum num of levels
    const char          *name;              // not really used yet
    int                  opposite;          // inc/dec pairing
    const char          *desc[3];           // messages for display screen
    const char          *gain[3];           // messages for gaining levels
    const char          *lose[3];           // messages for losing levels
};

static int Mutation_index[NUM_MUTATIONS];
static mutation_def Mutation_prop[] =
{
    // this must always be first (in order to catch non-existing mutations)
    { MUT_NONE, 0, 0, 0, "non mutation", MUT_NONE,
        {"Buggy mutation.",
            "Buggy mutation.",
            "Buggy mutation."},
        {"This mutation shouldn't happen.",
            "This mutation shouldn't happen.",
            "This mutation shouldn't happen."},
        {"This mutation shouldn't happen.",
            "This mutation shouldn't happen.",
            "This mutation shouldn't happen."}},

    { MUT_STRONG, 8, 10, 14, "strength", MUT_WEAK,
        {"Your muscles are strong (Str +", "", ""},
        {"You feel stronger.", "", ""},
        {"You feel weaker.", "", ""}},

    { MUT_CLEVER, 8, 10, 14, "intelligence", MUT_DOPEY,
        {"Your mind is acute (Int +", "", ""},
        {"You feel clever.", "", ""},
        {"You feel stupid.", "", ""}},

    { MUT_AGILE, 8, 10, 14, "dexterity", MUT_CLUMSY,
        {"You are agile (Dex +", "", ""},
        {"You feel agile.", "", ""},
        {"You feel clumsy.", "", ""}},

    { MUT_WEAK, 10, 10, 14, "weakness", MUT_STRONG,
        {"You are weak (Str -", "", ""},
        {"You feel weaker.", "", ""},
        {"You feel stronger.", "", ""}},

    { MUT_DOPEY, 10, 10, 14, "dopiness", MUT_CLEVER,
        {"You are dopey (Int -", "", ""},
        {"You feel stupid.", "", ""},
        {"You feel clever.", "", ""}},

    { MUT_CLUMSY, 10, 10, 14, "clumsiness", MUT_AGILE,
        {"You are clumsy (Dex -", "", ""},
        {"You feel clumsy.", "", ""},
        {"You feel agile.", "", ""}},

    { MUT_TOUGH_SKIN, 10, 10, 3, "tough skin", MUT_NONE,
        {"You have tough skin (AC +1).",
            "You have very tough skin (AC +3).",
            "You have extremely tough skin (AC +5)."},
        {"Your skin toughens.",
            "Your skin toughens.",
            "Your skin toughens."},
        {"Your skin feels delicate.",
            "Your skin feels delicate.",
            "Your skin feels delicate."}},

    { MUT_REPULSION_FIELD, 1, 1, 3, "repulsion field", MUT_NONE,
        {"You are surrounded by a mild repulsion field (EV + 1).",
            "You are surrounded by a moderate repulsion field (EV + 3).",
            "You are surrounded by a strong repulsion field (EV + 5; repel missiles)."},
        {"You begin to radiate repulsive energy.",
            "Your repulsive radiation grows stronger.",
            "Your repulsive radiation grows stronger."},
        {"You feel attractive.",
            "You feel attractive.",
            "You feel attractive."}},

    { MUT_POISON_RESISTANCE, 4, 8, 3, "poison resistance",  MUT_NONE,
        {"Your system is resistant to poisons.",
            "Your system is very resistant to poisons.",
            "Your system is almost immune to poisons."},
        {"You feel healthy.",
            "You feel healthy.",
            "You feel healthy."},
        {"You feel a little less healthy.",
            "You feel a little less healthy.",
            "You feel a little less healthy."}},

    { MUT_CARNIVOROUS, 5, 8, 3, "carnivorous",  MUT_HERBIVOROUS,
        {"Your digestive system is specialised to digest meat.",
            "Your digestive system is very specialised to digest meat.",
            "You are primarily a carnivore."},
        {"You hunger for flesh.",
            "You hunger for flesh.",
            "You hunger for flesh."},
        {"You feel able to eat a more balanced diet.",
            "You feel able to eat a more balanced diet.",
            "You feel able to eat a more balanced diet."}},

    { MUT_HERBIVOROUS, 5, 8, 3, "herbivorous",  MUT_CARNIVOROUS,
        {"You digest meat inefficiently.",
            "You digest meat very inefficiently.",
            "You are primarily a herbivore."},
        {"You hunger for vegetation.",
            "You hunger for vegetation.",
            "You hunger for vegetation."},
        {"You feel able to eat a more balanced diet.",
            "You feel able to eat a more balanced diet.",
            "You feel able to eat a more balanced diet."}},

    { MUT_HEAT_RESISTANCE, 4, 1, 3, "heat resistance", MUT_COLD_RESISTANCE,
        {"Your flesh is heat resistant.",
            "Your flesh is very heat resistant (but susceptible to cold).",
            "Your flesh is almost immune to the effects of heat (but susceptible to cold)."},
        {"You feel a sudden chill.",
            "You feel a sudden chill.",
            "You feel a sudden chill."},
        {"You feel hot for a moment.",
            "You feel hot for a moment.",
            "You feel hot for a moment."}},

    { MUT_COLD_RESISTANCE, 4, 1, 3, "cold resistance", MUT_HEAT_RESISTANCE,
        {"Your flesh is cold resistant.",
            "Your flesh is very cold resistant (but susceptible to heat).",
            "Your flesh is almost immune to the effects of cold (but susceptible to heat)."},
        {"You feel hot for a moment.",
            "You feel hot for a moment.",
            "You feel hot for a moment."},
        {"You feel a sudden chill.",
            "You feel a sudden chill.",
            "You feel a sudden chill."}},

    { MUT_SHOCK_RESISTANCE, 2, 0, 1, "shock resistance", MUT_NONE,
        {"Your flesh is restistant to electricity.", "", ""},
        {"You feel insulated.", "", ""},
        {"You feel conductive.", "", ""}},

    { MUT_FAST_METABOLISM, 10, 10, 3, "fast metabolism", MUT_SLOW_METABOLISM,
        {"You have a fast metabolism.",
            "You have a very fast metabolism.",
            "Your metabolism is lightning-fast."},
        {"You feel a little hungry.",
            "You feel a little hungry.",
            "You feel a little hungry."},
        {"Your metabolism slows.",
            "Your metabolism slows.",
            "Your metabolism slows."}},

    { MUT_SLOW_METABOLISM, 7, 7, 3, "slow metabolism", MUT_FAST_METABOLISM,
        {"You have a slow metabolism.",
            "You have a very slow metabolism.",
            "You need consume almost no food."},
        {"Your metabolism slows.",
            "Your metabolism slows.",
            "Your metabolism slows."},
        {"You feel a little hungry.",
            "You feel a little hungry.",
            "You feel a little hungry."}},

    { MUT_TELEPORT_CONTROL, 2, 4, 3, "teleport control", MUT_NONE,
        {"You can control long translocations somewhat.",
            "You can control long translocations reasonably well.",
            "You can control most translocations somewhat."},
        {"You feel controlled.",
            "You feel controlled.",
            "You feel controlled."},
        {"You feel random.",
            "You feel uncontrolled.",
            "You feel uncontrolled."}},

    { MUT_TELEPORT, 3, 6, 3, "random teleportation", MUT_NONE,
        {"Space occasionally distorts in your vicinity.",
            "Space sometimes distorts in your vicinity.",
            "Space frequently distorts in your vicinity."},
        {"You feel weirdly uncertain.",
            "You feel even more weirdly uncertain.",
            "You feel even more weirdly uncertain."},
        {"You feel stable.",
            "You feel stable.",
            "You feel stable."}},

    { MUT_MAGIC_RESISTANCE, 5, 8, 3, "magic resistance", MUT_NONE,
        {"You are resistant to magic.",
            "You are highly resistant to magic.",
            "You are extremely resistant to the effects of magic."},
        {"You feel resistant to magic.",
            "You feel more resistant to magic.",
            "You feel almost impervious to the effects of magic."},
        {"You feel less resistant to magic.",
            "You feel less resistant to magic.",
            "You feel vulnerable to magic again."}},

    { MUT_FAST, 1, 1, 3, "quick movement", MUT_NONE,
        {"You cover the ground quickly.",
            "You cover the ground very quickly.",
            "You cover the ground extremely quickly."},
        {"You feel quick.",
            "You feel quick.",
            "You feel quick."},
        {"You feel sluggish.",
            "You feel sluggish.",
            "You feel sluggish."}},

    { MUT_DEFORMED, 8, 10, 3, "deformed body", MUT_NONE,
        {"Armour fits poorly on your deformed body.",
            "Armour fits poorly on your badly deformed body.",
            "Armour fits poorly on your hideously deformed body."},
        {"Your body twists and deforms.",
            "Your body twists and deforms.",
            "Your body twists and deforms."},
        {"Your body's shape seems more normal.",
            "Your body's shape seems slightly more normal.",
            "Your body's shape seems slightly more normal."}},

    { MUT_TELEPORT_AT_WILL, 2, 2, 3, "teleport at will", MUT_NONE,
        {"You can teleport at will.",
            "You are good at teleporting at will.",
            "You can teleport instantly at will."},
        {"You feel jumpy.",
            "You feel more jumpy.",
            "You feel even more jumpy."},
        {"You feel static.",
            "You feel less jumpy.",
            "You feel less jumpy."}},

    { MUT_SPIT_POISON, 8, 10, 3, "spit poison", MUT_NONE,
        {"You can spit poison.",
            "You can spit strong poison.",
            "You can exhale a cloud of poison."},
        {"There is a nasty taste in your mouth for a moment.",
            "There is a nasty taste in your mouth for a moment.",
            "There is a nasty taste in your mouth for a moment."},
        {"You feel an ache in your throat.",
            "You feel an ache in your throat.",
            "You feel an ache in your throat."}},

    { MUT_MAPPING, 3, 6, 3, "detect surroundings", MUT_NONE,
        {"You can sense your immediate surroundings.",
            "You can sense your surroundings.",
            "You can sense a large area of your surroundings."},
        {"You feel aware of your surroundings.",
            "You feel more aware of your surroundings.",
            "You feel even more aware of your surroundings."},
        {"You feel slightly disorientated.",
            "You feel slightly disorientated.",
            "You feel slightly disorientated."}},

    { MUT_BREATHE_FLAMES, 4, 6, 3, "fire breath", MUT_NONE,
        {"You can breathe flames.",
            "You can breathe fire.",
            "You can breathe blasts of fire."},
        {"Your throat feels hot.",
            "Your throat feels hot.",
            "Your throat feels hot."},
        {"A chill runs up and down your throat.",
            "A chill runs up and down your throat.",
            "A chill runs up and down your throat."}},

    { MUT_BLINK, 3, 6, 3, "blinking", MUT_NONE,
        {"You can translocate small distances instantaneously.",
            "You can translocate small distances instantly, with a minimum distance.",
            "You can translocate small distances instantly, even through walls sometimes."},
        {"You feel a little jumpy.",
            "You feel more jumpy.",
            "You feel even more jumpy."},
        {"You feel a little less jumpy.",
            "You feel less jumpy.",
            "You feel less jumpy."}},

    { MUT_STRONG_STIFF, 10, 10, 3, "strong, stiff muscles", MUT_FLEXIBLE_WEAK,
        {"Your muscles are strong (Str +1), but stiff (Dex -1).",
            "Your muscles are very strong (Str +2), but stiff (Dex -2).",
            "Your muscles are extremely strong (Str +3), but stiff (Dex -3)."},
        {"Your muscles feel sore.",
            "Your muscles feel sore.",
            "Your muscles feel sore."},
        {"Your muscles feel loose.",
            "Your muscles feel loose.",
            "Your muscles feel loose."}},

    { MUT_FLEXIBLE_WEAK, 10, 10, 3, "flexible, weak muscles", MUT_STRONG_STIFF,
        {"Your muscles are flexible (Dex +1), but weak (Str -1).",
            "Your muscles are very flexible (Dex +2), but weak (Str -2).",
            "Your muscles are extremely flexible (Dex +3), but weak (Str -3)."},
        {"Your muscles feel loose.",
            "Your muscles feel loose.",
            "Your muscles feel loose."},
        {"Your muscles feel sore.",
            "Your muscles feel sore.",
            "Your muscles feel sore."}},

    { MUT_LOST, 6, 8, 3, "forgetfulness", MUT_CLARITY,
        {"You occasionally forget where you are.",
            "You sometimes forget where you are.",
            "You frequently forget where you are."},
        {"You feel a little disoriented.",
            "You feel a little disoriented.",
            "Where the Hells are you?"},
        {"You feel less disoriented.",
            "You feel less disoriented.",
            "You feel less disoriented."}},

    { MUT_CLARITY, 6, 8, 3, "mental clarity", MUT_LOST,
        {"You possess an exceptional clarity of mind.",
            "You possess an unnatural clarity of mind.",
            "You possess a supernatural clarity of mind."},
        {"Your thoughts seem clearer.",
            "Your thoughts seem clearer.",
            "Your thoughts seem clearer."},
        {"Your thinking seems confused.",
            "Your thinking seems confused.",
            "Your thinking seems confused."}},

    { MUT_BERSERK, 7, 7, 3, "berserk temper", MUT_NONE,
        {"You tend to lose your temper in combat.",
            "You often lose your temper in combat.",
            "You have an uncontrollable temper."},
        {"You feel a little pissed off.",
            "You feel angry.",
            "You feel extremely angry at everything!"},
        {"You feel a little more calm.",
            "You feel a little less angry.",
            "You feel a little less angry."}},

    { MUT_REGENERATION, 3, 3, 3, "regeneration", MUT_DETERIORATION,
        {"Your natural rate of healing is unusually fast.",
            "You heal very quickly.",
            "You regenerate."},
        {"You begin to heal more quickly.",
            "You begin to heal more quickly.",
            "You begin to regenerate."},
        {"Your rate of healing slows.",
            "Your rate of healing slows.",
            "Your rate of healing slows."}},

    { MUT_DETERIORATION, 10, 10, 3, "deterioration", MUT_REGENERATION,
        {"Your body is slowly deteriorating.",
            "Your body is deteriorating.",
            "Your body is rapidly deteriorating."},
        {"You feel yourself wasting away.",
            "You feel yourself wasting away.",
            "You feel your body start to fall apart."},
        {"You feel healthier.",
            "You feel a little healthier.",
            "You feel a little healthier."}},

    { MUT_ACUTE_VISION, 4, 8, 3, "acute vision", MUT_BLURRY_VISION,
        {"You have excellent eyesight and can find hidden things easier.",
            "You have magically sharp eyesight and heighened awareness.",
            "You have supernaturally acute eyesight and can see the invisible."},
        {"Your vision sharpens.",
            "Your vision sharpens.",
            "Your vision sharpens."},
        {"Your vision seems duller.",
            "Your vision seems duller.",
            "Your vision seems duller."}},

    { MUT_BLURRY_VISION, 10, 10, 3, "blurry vision", MUT_ACUTE_VISION,
        {"Your vision is a little blurry.",
            "Your vision is quite blurry.",
            "Your vision is extremely blurry."},
        {"Your vision blurs.",
            "Your vision blurs.",
            "Your vision blurs."},
        {"Your vision sharpens.",
            "Your vision sharpens a little.",
            "Your vision sharpens a little."}},

    { MUT_MUTATION_RESISTANCE, 4, 8, 3, "mutation resistance", MUT_NONE,
        {"You are somewhat resistant to further mutation.",
            "You are somewhat resistant to both further mutation and mutation removal.",
            "Your current mutations are irrevocably fixed, and you can mutate no more."},
        {"You feel genetically stable.",
            "You feel genetically stable.",
            "You feel genetically immutable."},
        {"You feel genetically unstable.",
            "You feel genetically unstable.",
            "You feel genetically unstable."}},

    { MUT_FRAIL, 10, 10, 3, "frailty", MUT_ROBUST,
        {"You are frail (-10 percent hp).",
            "You are very frail (-20 percent hp).",
            "You are extremely frail (-30 percent hp)."},
        {"You feel frail.",
            "You feel frail.",
            "You feel frail."},
        {"You feel robust.",
            "You feel robust.",
            "You feel robust."}},

    { MUT_ROBUST, 5, 3, 3, "robustness", MUT_FRAIL,
        {"You are robust (+10 percent hp).",
            "You are very robust (+20 percent hp).",
            "You are extremely robust (+30 percent hp)."},
        {"You feel robust.",
            "You feel robust.",
            "You feel robust."},
        {"You feel frail.",
            "You feel frail.",
            "You feel frail."}},

    { MUT_NEGATIVE_ENERGY_RESISTANCE, 3, 6, 3, "negative energy resistance", MUT_NONE,
        {"You resist negative energy.",
            "You are quite resistant to negative energy.",
            "You are immune to negative energy."},
        {"You feel negative.",
            "You feel negative.",
            "You feel negative."},
        {"You feel vulnerable to negative energy.",
            "You feel more vulnerable to negative energy.",
            "You feel more vulnerable to negative energy."}},

        // Some demonic powers start here (note the zero rarity and the
        // lack of unneeded loss messages).
    { MUT_TORMENT_RESISTANCE, 0, 0, 3, "pain resistance", MUT_NONE,
        {"You are resistant to unholy pain.",
            "You are resistant to torment.",
            "You are immune to unholy pain and torment." },
        {"You feel a strange anaesthesia.",
            "You feel a strange anaesthesia.",
            "You feel a strange anaesthesia."},
        {"", "", ""}},

    { MUT_SUMMON_DEMONS, 0, 0, 3, "summon demons", MUT_NONE,
        {"You can summon a minor demon to your aid.",
            "You can summon demons to your aid.",
            "You can control demons."},
        { "Help is not far away!",
            "A thousand chattering voices call out to you.",
            "The armies of all the hells are at your call." },
        {"", "", ""}},

    { MUT_RAISE_DEAD, 0, 0, 3, "raise dead", MUT_NONE,
        {"You can raise the dead serve you.",
            "You can better raise the dead to serve you.",
            "You can enslave the undead." },
        {"You feel an affinity for the dead.",
            "The dead await your call.",
            "The armies of the undead are at your command." },
        {"", "", ""}},

    { MUT_PANDEMONIUM, 0, 0, 3, "gate to pandemonium", MUT_NONE,
        {"You can travel to (but not from) Pandemonium at will.",
            "You can travel to Pandemonium, and find exits much easier than most.",
            "You can travel from one level of Pandemonium to another, with difficulty."},
        {"You feel something pulling you to a strange and terrible place.",
            "You feel something pulling you in a new, crazed direction.",
            "The infinite planes of Pandimonium lay open before you."},
        {"", "", ""}},

    { MUT_DEATH_STRENGTH, 0, 0, 3, "death strength", MUT_NONE,
        {"You can draw strength from the death of the living.",
            "You can draw strength more often from the death of the living.",
            "You draw strength frequently from the death of the living."},
        {"You hunger for death.",
            "You thirst for death.",
            "You thrive on the death of the living."},
        {"", "", ""}},

    { MUT_CHANNEL_HELL, 0, 0, 3, "magic channelling", MUT_NONE,
        {"You can channel small amounts of magical energy from Hell.",
            "You can channel magical energy from Hell.",
            "Your magic regenerates faster by drawing on the powers of Hell."},
        {"You feel a flux of magical energy.",
            "You feel a stronger flux of magic.",
            "Wild fluxes of magical energy surge around you."},
        {"", "", ""}},

    { MUT_CALL_TORMENT, 0, 0, 3, "call torment", MUT_NONE,
        {"You can inflict pain on your enemies.",
            "You can invoke the powers of Tartarus to smite your living foes.",
            "You can call on the torments of Hell." },
        {"You feel a sudden searing pain.",
            "A shadow passes over the world around you.",
            "You feel a terrifying power at your call." },
        {"", "", ""}},

    { MUT_THROW_FLAMES, 0, 0, 3, "throw flames", MUT_NONE,
        {"You can throw forth bursts of the flames of Gehenna.",
            "You can throw forth blasts of fire from Gehenna.",
            "You can hurl mighty blasts of hellfire." },
        {"You smell the fires of Gehenna.",
            "Your breath smells like brimstone.",
            "Dark fire rises from your fingertips."},
        {"", "", ""}},

    { MUT_THROW_FROST, 0, 0, 3, "throw frost", MUT_NONE,
        {"You can throw forth the frost of Cocytus.",
            "You can throw forth blasts of cold from Cocytus.",
            "You can hurl mighty blasts of hellfrost."},
        { "Your breath forms into a cold vapour.",
            "Everything you touch is encased in frost.",
            "You feel the icy cold of Cocytus chill your soul." },
        {"", "", ""}},

    { MUT_DRAIN_LIFE, 0, 0, 3, "drain life", MUT_NONE,
        {"You can do extra damage with a painful touch in unarmed combat.",
            "You can drain life in unarmed combat.",
            "You have a vampiric touch."},
        {"Your skin tingles in a strangely unpleasant way.",
            "Your skin tingles in a strangely unpleasant way.",
            "Your skin tingles in a strangely unpleasant way." },
        {"", "", ""}},

    { MUT_CLAWS, 2, 5, 3, "claws", MUT_NONE,
        {"You have sharp fingernails.",
            "Your fingernails are very sharp.",
            "You have claws for hands."},
        {"Your fingernails lengthen.",
            "Your fingernails sharpen.",
            "Your hands twist into claws."},
        {"Your fingernails shrink to normal size.",
            "Your fingernails look duller.",
            "Your hands feel fleshier."}},

    { MUT_HORNS, 7, 7, 3, "horns", MUT_NONE,
        {"You have a pair of small horns on your head.",
            "You have a pair of horns on your head.",
            "You have a pair of large horns on your head."},
        {"A pair of horns grows on your head!",
            "The horns on your head grow some more.",
            "The horns on your head grow some more."},
        {"The horns on your head shrink away.",
            "The horns on your head shrink a bit.",
            "The horns on your head shrink a bit."}},

    { MUT_HOOVES, 2, 5, 3, "hooves",  MUT_NONE,
        {"You have powerful kicking legs.",
            "You have powerful kicking legs and hooves.",
            "You run swiftly on your powerful legs and hooves."},
        {"Your legs become stronger.",
            "Your feet shrivel into cloven hooves.",
            "Your legs become swifter."},
        {"Your legs become weaker.",
            "Your hooves expand and flesh out into feet!",
            "You can no longer run swiftly on your hooves."}},

    { MUT_STINGER, 0, 3, 3, "stinger", MUT_NONE,          //jmf: nagas & dracos
        {"Your tail ends in a poisonous barb.",
            "Your tail ends in a sharp poisonous barb.",
            "Your tail ends in a wicked poisonous barb."},
        {"A poisonous barb forms on the end of your tail.",
            "The barb on your tail looks sharper.",
            "The barb on your tail looks very sharp."},
        {"The barb on your tail disappears.",
            "The barb on your tail looks duller.",
            "The barb on your tail looks duller."}},

    { MUT_BIG_WINGS, 0, 0, 1, "big wings", MUT_NONE,      //jmf: dracos only
        {"Your wings are large and strong.", "", ""},
        {"Your wings grow larger and stronger.", "", ""},
        {"Your wings shrink back to their normal size.", "", ""}},

    { MUT_GREEN_SCALES, 2, 5, 3, "green scales", MUT_NONE,
        {"You are partially covered in green scales (AC +2).",
            "You are mostly covered in green scales (AC +3).",
            "You are covered in green scales (AC +4, resist poison)."},
        {"Green scales grow over part of your body.",
            "Green scales spread over more of your body.",
            "Green scales cover you completely."},
        {"Your green scales disappear.",
            "Your green scales recede somewhat.",
            "Your green scales recede somewhat."}},

    { MUT_BLACK_SCALES, 1, 3, 3, "thick black scales", MUT_NONE,
        {"You are partially covered in thick black scales (AC +3, EV -1).",
            "You are mostly covered in thick black scales (AC +5, EV -2).",
            "You are completely covered in thick black scales (AC +7, EV -3)."},
        {"Thick black scales grow over part of your body.",
            "Thick black scales spread over more of your body.",
            "Thick black scales cover you completely."},
        {"Your black scales disappear.",
            "Your black scales recede somewhat.",
            "Your black scales recede somewhat."}},

    { MUT_GREY_SCALES, 2, 5, 3, "supple grey scales", MUT_NONE,
        {"You are partially covered in supple grey scales (AC +1).",
            "You are mostly covered in supple grey scales (AC +2).",
            "You are completely covered in supple grey scales (AC +3, regeneration)."},
        {"Supple grey scales grow over part of your body.",
            "Supple grey scales spread over more of your body.",
            "Supple grey scales cover you completely."},
        {"Your grey scales disappear.",
            "Your grey scales recede somewhat.",
            "Your grey scales recede somewhat."}},

    { MUT_BONEY_PLATES, 1, 3, 3, "boney plates", MUT_NONE,
        {"You are protected by plates of bone (AC +2, EV -1).",
            "You are protected by plates of bone (AC +4, EV -2).",
            "You are protected by plates of bone (AC +6, EV -3)."},
        {"You grow protective plates of bone.",
            "You grow more protective plates of bone.",
            "You grow more protective plates of bone."},
        {"Your bony plates shrink away.",
            "Your bony plates shrink.",
            "Your bony plates shrink."}},

    { MUT_RED_SCALES, 2, 5, 3, "red scales", MUT_NONE,
        {"You are partially covered in red scales (AC +1).",
            "You are mostly covered in red scales (AC +2).",
            "You are covered in red scales (AC +3, resist fire)."},
        {"Red scales grow over part of your body.",
            "Red scales spread over more of your body.",
            "Red scales cover you completely."},
        {"Your red scales disappear.",
            "Your red scales recede somewhat.",
            "Your red scales recede somewhat."}},

    { MUT_NACREOUS_SCALES, 1, 3, 3, "smooth nacreous scales", MUT_NONE,
        {"You are partially covered in smooth nacreous scales (AC +1).",
            "You are mostly covered in smooth nacreous scales (AC +3).",
            "You are completely covered in smooth nacreous scales (AC +5)."},
        {"Smooth nacreous scales grow over part of your body.",
            "Smooth nacreous scales spread over more of your body.",
            "Smooth nacreous scales cover you completely."},
        {"Your smooth nacreous scales disappear.",
            "Your smooth nacreous scales recede somewhat.",
            "Your smooth nacreous scales recede somewhat."}},

    { MUT_GREY2_SCALES, 2, 5, 3, "ridged grey scales", MUT_NONE,
        {"You are partially covered in ridged grey scales (AC +3, EV -1).",
            "You are mostly covered in ridged grey scales (AC +4, EV -1).",
            "You are completely covered in ridged grey scales (AC +6, EV -2)."},
        {"Ridged grey scales grow over part of your body.",
            "Ridged grey scales spread over more of your body.",
            "Ridged grey scales cover you completely."},
        {"Your ridged grey scales disappear.",
            "Your ridged grey scales recede somewhat.",
            "Your ridged grey scales recede somewhat."}},

    { MUT_METALLIC_SCALES, 1, 3, 3, "metallic scales", MUT_NONE,
        {"You are partially covered in metallic scales (AC +3, EV -1).",
            "You are mostly covered in metallic scales (AC +5, EV -2).",
            "You are completely covered in metallic scales (AC +8, EV -4)."},
        {"Metallic scales grow over part of your body.",
            "Metallic scales spread over more of your body.",
            "Metallic scales cover you completely."},
        {"Your metallic scales disappear.",
            "Your metallic scales recede somewhat.",
            "Your metallic scales recede somewhat."}},

    { MUT_BLACK2_SCALES, 2, 5, 3, "black scales",  MUT_NONE,
        {"You are partially covered in black scales (AC +1).",
            "You are mostly covered in black scales (AC +2).",
            "You are completely covered in black scales (AC +4, resist negative energy)."},
        {"Black scales grow over part of your body.",
            "Black scales spread over more of your body.",
            "Black scales cover you completely."},
        {"Your black scales disappear.", "Your black scales recede somewhat.",
            "Your black scales recede somewhat."}},

    { MUT_WHITE_SCALES, 2, 5, 3, "white scales", MUT_NONE,
        {"You are partially covered in white scales (AC +1).",
            "You are mostly covered in white scales (AC +2).",
            "You are completely covered in white scales (AC +3, resist cold)."},
        {"White scales grow over part of your body.",
            "White scales spread over more of your body.",
            "White scales cover you completely."},
        {"Your white scales disappear.",
            "Your white scales recede somewhat.",
            "Your white scales recede somewhat."}},

    { MUT_YELLOW_SCALES, 2, 5, 3, "yellow scales", MUT_NONE,
        {"You are partially covered in yellow scales (AC +1).",
            "You are mostly covered in yellow scales (AC +3).",
            "You are completely covered in yellow scales (AC +4, resist acid)."},
        {"Yellow scales grow over part of your body.",
            "Yellow scales spread over more of your body.",
            "Yellow scales cover you completely."},
        {"Your yellow scales disappear.",
            "Your yellow scales recede somewhat.",
            "Your yellow scales recede somewhat."}},

    { MUT_BROWN_SCALES, 2, 5, 3, "brown scales", MUT_NONE,
        {"You are partially covered in brown scales (AC +2).",
            "You are mostly covered in brown scales (AC +4).",
            "You are completely covered in brown scales (AC +5)."},
        {"Brown scales grow over part of your body.",
            "Brown scales spread over more of your body.",
            "Brown scales cover you completely."},
        {"Your brown scales disappear.",
            "Your brown scales recede somewhat.",
            "Your brown scales recede somewhat."}},

    { MUT_BLUE_SCALES, 2, 5, 3, "blue scales", MUT_NONE,
        {"You are partially covered in blue scales (AC +1).",
            "You are mostly covered in blue scales (AC +2).",
            "You are completely covered in blue scales (AC +3, resist electricity)."},
        {"Blue scales grow over part of your body.",
            "Blue scales spread over more of your body.",
            "Blue scales cover you completely."},
        {"Your blue scales disappear.",
            "Your blue scales recede somewhat.",
            "Your blue scales recede somewhat."}},

    { MUT_PURPLE_SCALES, 2, 5, 3, "purple scales", MUT_NONE,
        {"You are partially covered in purple scales (AC +2).",
            "You are mostly covered in purple scales (AC +3).",
            "You are completely covered in purple scales (AC +4, resist magic)."},
        {"Purple scales grow over part of your body.",
            "Purple scales spread over more of your body.",
            "Purple scales cover you completely."},
        {"Your purple scales disappear.",
            "Your purple scales recede somewhat.",
            "Your purple scales recede somewhat."}},

    { MUT_SPECKLED_SCALES, 2, 5, 3, "speckled scales", MUT_NONE,
        {"You are partially covered in speckled scales (AC +2).",
            "You are mostly covered in speckled scales (AC +4, EV -1).",
            "You are covered in speckled scales (AC +6, EV -1)."},
        {"Speckled scales grow over part of your body.",
            "Speckled scales spread over more of your body.",
            "Speckled scales cover you completely."},
        {"Your speckled scales disappear.",
            "Your speckled scales recede somewhat.",
            "Your speckled scales recede somewhat."}},

    { MUT_ORANGE_SCALES, 2, 5, 3, "orange scales", MUT_NONE,
        {"You are partially covered in orange scales (AC +3).",
            "You are mostly covered in orange scales (AC +4).",
            "You are completely covered in orange scales (AC +5)."},
        {"Orange scales grow over part of your body.",
            "Orange scales spread over more of your body.",
            "Orange scales cover you completely."},
        {"Your orange scales disappear.",
            "Your orange scales recede somewhat.",
            "Your orange scales recede somewhat."}},

    { MUT_INDIGO_SCALES, 2, 5, 3, "indigo scales", MUT_NONE,
        {"You are partially covered in indigo scales (AC +2).",
            "You are mostly covered in indigo scales (AC +3).",
            "You are completely covered in indigo scales (AC +5)."},
        {"Indigo scales grow over part of your body.",
            "Indigo scales spread over more of your body.",
            "Indigo scales cover you completely."},
        {"Your indigo scales disappear.",
            "Your indigo scales recede somewhat.",
            "Your indigo scales recede somewhat."}},

    { MUT_RED2_SCALES, 1, 3, 3, "knobbly red scales", MUT_NONE,
        {"You are partially covered in knobbly red scales (AC +2).",
            "You are mostly covered in knobbly red scales (AC +5, EV -1).",
            "You are completely covered in knobbly red scales (AC +7, EV -2)."},
        {"Knobbly red scales grow over part of your body.",
            "Knobbly red scales spread over more of your body.",
            "Knobbly red scales cover you completely."},
        {"Your knobbly red scales disappear.",
            "Your knobbly red scales recede somewhat.",
            "Your knobbly red scales recede somewhat."}},

    { MUT_IRIDESCENT_SCALES, 1, 3, 3, "iridescent scales", MUT_NONE,
        {"You are partially covered in iridescent scales (AC +1).",
            "You are mostly covered in iridescent scales (AC +2, EV +1).",
            "You are completely covered in iridescent scales (AC +3, EV +3)."},
        {"Iridescent scales grow over part of your body.",
            "Iridescent scales spread over more of your body.",
            "Iridescent scales cover you completely."},
        {"Your iridescent scales disappear.",
            "Your iridescent scales recede somewhat.",
            "Your iridescent scales recede somewhat."}},

    { MUT_PATTERNED_SCALES, 1, 3, 3, "patterned scales", MUT_NONE,
        {"You are partially covered in patterned scales (AC +1).",
            "You are mostly covered in patterned scales (AC +2, stealth).",
            "You are completely covered in patterned scales (AC +4, stealth)."},
        {"Patterned scales grow over part of your body.",
            "Patterned scales spread over more of your body.",
            "Patterned scales cover you completely."},
        {"Your patterned scales disappear.",
            "Your patterned scales recede somewhat.",
            "Your patterned scales recede somewhat."}},
};

const int NUM_ACTUAL_MUTATIONS = sizeof(Mutation_prop) / sizeof(mutation_def);

void init_mutations( void )
{
    ASSERT( NUM_ACTUAL_MUTATIONS <= NUM_MUTATIONS );

    int i;

    // init all mutations to point to element #0 (the fake one)
    for (i = 0; i < NUM_MUTATIONS; i++)
        Mutation_index[i] = 0;

    // skip over the fake one and build the index table
    for (i = 1; i < NUM_ACTUAL_MUTATIONS; i++)
        Mutation_index[ Mutation_prop[i].type ] = i;
}

static const mutation_def & get_mutation_info( int which_mutat )
{
    ASSERT( which_mutat >= 0 && which_mutat < NUM_MUTATIONS );

    return (Mutation_prop[ Mutation_index[which_mutat] ]);
}

static inline void gain_msg( int mutat, int level )
{
    ASSERT( mutat >= 0 && mutat < NUM_MUTATIONS );

    const mutation_def &mut = get_mutation_info( mutat );

    if (mutat == MUT_STRONG || mutat == MUT_CLEVER || mutat == MUT_AGILE
        || mutat == MUT_WEAK || mutat == MUT_DOPEY || mutat == MUT_CLUMSY)
    {
        level = 0;
    }

    ASSERT( level >= 0 && level < 3 );

    const char *const msg = mut.gain[level];

    if (msg[0] != '\0')
        mpr( MSGCH_MUTATION, msg );
    else
        mpr( MSGCH_MUTATION, "You feel more abnormal." );
}

static inline void lose_msg( int mutat, int level )
{
    ASSERT( mutat >= 0 && mutat < NUM_MUTATIONS );

    const mutation_def &mut = get_mutation_info( mutat );

    if (mutat == MUT_STRONG || mutat == MUT_CLEVER || mutat == MUT_AGILE
        || mutat == MUT_WEAK || mutat == MUT_DOPEY || mutat == MUT_CLUMSY)
    {
        level = 0;
    }

    ASSERT( level >= 0 && level < 3 );

    const char *const msg = mut.lose[level];

    if (msg[0] != '\0')
        mpr( MSGCH_MUTATION, msg );
    else
        mpr( MSGCH_MUTATION, "You feel less abnormal." );
}

static inline int mutation_rarity( int mutat )
{
    ASSERT( mutat >= 0 && mutat < NUM_MUTATIONS );

    const mutation_def &mut = get_mutation_info( mutat );

    return ((you.mutation[mutat] > 0) ? mut.inc_rarity : mut.rarity);
}

static int how_mutated(void);
static bool demon_mutate( int which_mut, int levels );

static bool is_scale_mutation( int mut )
{
    return (mut == MUT_TOUGH_SKIN
            || (mut >= MUT_GREEN_SCALES && mut <= MUT_BONEY_PLATES)
            || (mut >= MUT_RED_SCALES && mut <= MUT_PATTERNED_SCALES));
}

static int get_scale_type( void )
{
    if (you.species == SP_NAGA || player_genus( GENPC_DRACONIAN ))
        return (MUT_INTRINSIC);

    for (int i = 0; i < NUM_MUTATIONS; i++)
    {
        if (you.mutation[i] && is_scale_mutation(i))
            return (i);
    }

    return (MUT_NONE);
}

static mutation_type pick_random_scale_type( void )
{
    int count = 0;
    mutation_type pick = MUT_NONE;

    for (int i = 0; i < NUM_MUTATIONS; i++)
    {
        if (is_scale_mutation(i))
        {
            const int rare = mutation_rarity(i);

            count += rare;
            if (random2( count ) < rare)
                pick = static_cast<mutation_type>( i );
        }
    }

    return (pick);
}

static int body_covered( void )
{
    /* checks how much of your body is covered by scales etc */
    int covered = 0;

    const int scales = get_scale_type();

    if (scales == MUT_INTRINSIC)
        covered += 3;
    else if (scales != MUT_NONE)
        covered += you.mutation[ scales ];

    return (covered);
}

int get_scale_property( armour_property_type prop )
{
    int ret = 0;

    if (transform_changed_physiology( true ))
        return (0);

    if (prop == PARM_AC)
    {
        // Scales -- some evil uses of the fact that boolean "true" == 1...
        // I'll spell things out with some comments -- bwr

        // mutations:
        // these give: +1, +2, +3
        ret += you.mutation[MUT_GREY_SCALES];
        ret += you.mutation[MUT_RED_SCALES];
        ret += you.mutation[MUT_WHITE_SCALES];
        ret += you.mutation[MUT_BLUE_SCALES];
        ret += you.mutation[MUT_IRIDESCENT_SCALES];

        // these give: +1, +2, +4
        ret += you.mutation[MUT_BLACK2_SCALES]
                    + (you.mutation[MUT_BLACK2_SCALES] == 3);
        ret += you.mutation[MUT_PATTERNED_SCALES]
                    + (you.mutation[MUT_PATTERNED_SCALES] == 3);

        // these give: +1, +3, +4
        ret += you.mutation[MUT_YELLOW_SCALES]
                    + (you.mutation[MUT_YELLOW_SCALES] > 1);

        // these give: +1, +3, +5
        if (you.mutation[MUT_NACREOUS_SCALES] > 0)
            ret += (you.mutation[MUT_NACREOUS_SCALES] * 2) - 1;
        if (you.mutation[MUT_TOUGH_SKIN] > 0)
            ret += (you.mutation[MUT_TOUGH_SKIN] * 2) - 1;

        // these give: +2, +3, +4
        if (you.mutation[MUT_GREEN_SCALES] > 0)
            ret += you.mutation[MUT_GREEN_SCALES] + 1;
        if (you.mutation[MUT_PURPLE_SCALES] > 0)
            ret += you.mutation[MUT_PURPLE_SCALES] + 1;

        // these give: +2, +3, +5
        if (you.mutation[MUT_INDIGO_SCALES] > 0)
        {
            ret += 1 + you.mutation[MUT_INDIGO_SCALES]
                     + (you.mutation[MUT_INDIGO_SCALES] == 3);
        }

        // these give: +2, +4, +5
        ret += (you.mutation[MUT_BROWN_SCALES] * 2)
                            - (you.mutation[MUT_BROWN_SCALES] == 3);

        // these give: +2, +4, +6
        ret += you.mutation[MUT_BONEY_PLATES] * 2;
        ret += you.mutation[MUT_GREY2_SCALES] * 2;

        // these give: +2, +5, +7
        ret += (you.mutation[MUT_RED2_SCALES] * 2)
                    + (you.mutation[MUT_RED2_SCALES] > 1);

        // these give: +3, +4, +5
        if (you.mutation[MUT_ORANGE_SCALES] > 0)
            ret += you.mutation[MUT_ORANGE_SCALES] + 2;

        // these give: +3, +4, +6
        if (you.mutation[MUT_SPECKLED_SCALES] > 0)
        {
            ret += you.mutation[MUT_SPECKLED_SCALES] * 2
                    + (you.mutation[MUT_SPECKLED_SCALES] == 1);
        }

        // these give: +3, +5, +7
        if (you.mutation[MUT_BLACK_SCALES] > 0)
            ret += 1 + you.mutation[MUT_BLACK_SCALES] * 2;

        // these give: +3, +5, +8
        if (you.mutation[MUT_METALLIC_SCALES] > 0)
        {
            ret += 1 + you.mutation[MUT_METALLIC_SCALES] * 2
                     + (you.mutation[MUT_METALLIC_SCALES] == 3);
        }
    }
    else if (prop == PARM_EVASION)
    {
        // these give: -1, -2, -3
        ret -= you.mutation[MUT_BLACK_SCALES];
        ret -= you.mutation[MUT_BONEY_PLATES];

        // these give: -1, -1, -2
        ret -= (you.mutation[MUT_GREY2_SCALES] + 1) / 2;

        // these give: -1, -2, -4
        ret -= (you.mutation[MUT_METALLIC_SCALES]
                    + (you.mutation[MUT_METALLIC_SCALES] == 3));

        // these give:  0, -1, -1
        ret -= (you.mutation[MUT_SPECKLED_SCALES] > 1);

        // these give:  0, -1, -2
        if (you.mutation[MUT_RED2_SCALES] > 1)
            ret -= (you.mutation[MUT_RED2_SCALES] - 1);

        // these give:  0, +1, +3
        if (you.mutation[MUT_IRIDESCENT_SCALES] > 1)
            ret += 2 * (you.mutation[MUT_IRIDESCENT_SCALES] - 1) - 1;
    }

    return (ret);
}

void display_mutations(void)
{
    int i;
    int j = 0;
    const char *mut_title = "Innate abilities, Weirdness & Mutations";
    const int num_lines = get_number_of_lines();

#ifdef DOS_TERM
    char buffer[4800];

    window(1, 1, 80, 25);
    gettext(1, 1, 80, 25, buffer);
#endif

    clrscr();
    textcolor(WHITE);

    // center title
    i = 40 - strlen(mut_title) / 2;
    if (i<1) i=1;
    gotoxy(i, 1);
    cprintf(mut_title);
    gotoxy(1,3);
    textcolor(LIGHTBLUE);  //textcolor for inborn abilities and weirdness

    switch (you.species)   //mv: following code shows innate abilities - if any
    {
    case SP_MERFOLK:
        cprintf("  You revert to your normal form in water." EOL);
        j++;
        break;

    case SP_NAGA:
        // breathe poison replaces spit poison:
        if (you.mutation[MUT_SPIT_POISON] == 3)
            cprintf("  You can exhale a cloud of poison." EOL);
        else
            cprintf("  You can spit poison." EOL);

        cprintf("  Your race is resistant to poisons." EOL);
        cprintf("  You can see invisible." EOL);
        j += 3;
        break;

    case SP_GNOME:
        cprintf("  You can sense your surroundings." EOL);
        j++;
        break;

    case SP_TROLL:
        cprintf("  Your body regenerates quickly." EOL);
        j++;
        break;

    case SP_GHOUL:
        cprintf("  Your body is rotting away." EOL);
        cprintf("  You are carnivorous." EOL);
        j += 2;
        break;

    case SP_KOBOLD:
        cprintf("  You are carnivorous." EOL);
        j++;
        break;

    case SP_GREY_ELF:
        if (you.xp_level > 4)
        {
            cprintf("  You are very charming." EOL);
            j++;
        }
        break;

    case SP_KENKU:
        if (you.xp_level > 4)
        {
            cprintf( "  You can fly" );
            cprintf( (you.xp_level > 14) ? " continuously." EOL
                                                 : "." EOL );

            j++;
        }
        break;

    case SP_MUMMY:
        cprintf("  You are");
        cprintf((you.xp_level > 25) ? " very strongly" :
                ((you.xp_level > 12) ? " strongly" : ""));
        cprintf(" in touch with the powers of death." EOL);
        j++;

        if (you.xp_level > 12)
        {
            cprintf("  You can restore your body by infusing magical energy." EOL);
            j++;
        }
        break;

    case SP_GREEN_DRACONIAN:
        if (you.xp_level >= 7)
        {
            if (you.xp_level >= 21)
                cprintf("  You are immune to poison." EOL);
            else if (you.xp_level >= 14)
                cprintf("  You are very resistant to poison." EOL);
            else
                cprintf("  You are resistant to poison." EOL);

            cprintf("  You can breathe poison." EOL);
            j += 2;
        }
        break;

    case SP_RED_DRACONIAN:
        if (you.xp_level > 6)
        {
            cprintf("  You can breathe fire." EOL);
            j++;
        }
        if (you.xp_level > 17)
        {
            cprintf("  You are resistant to fire." EOL);
            j++;
        }
        break;

    case SP_WHITE_DRACONIAN:
        if (you.xp_level > 6)
        {
            cprintf("  You can breathe frost." EOL);
            j++;
        }
        if (you.xp_level > 17)
        {
            cprintf("  You are resistant to cold." EOL);
            j++;
        }
        break;

    case SP_BLACK_DRACONIAN:
        if (you.xp_level > 6)
        {
            cprintf("  You can breathe lightning." EOL);
            j++;
        }
        if (you.xp_level > 17)
        {
            cprintf("  You are resistant to lightning." EOL);
            j++;
        }
        break;

    case SP_YELLOW_DRACONIAN:
        if (you.xp_level > 6)
        {
            cprintf("  You can spit acid." EOL);
            j++;
        }
        break;

    case SP_PURPLE_DRACONIAN:
        if (you.xp_level > 6)
        {
            cprintf("  You can breathe power." EOL);
            j++;
        }
        break;

    case SP_MOTTLED_DRACONIAN:
        if (you.xp_level > 6)
        {
            cprintf("  You can breathe sticky flames." EOL);
            j++;
        }
        break;

    case SP_PALE_DRACONIAN:
        if (you.xp_level > 6)
        {
            cprintf("  You can breathe steam." EOL);
            j++;
        }
        break;
    }                           //end switch - innate abilities

    textcolor(LIGHTGREY);

    for (i = 0; i < NUM_MUTATIONS; i++)
    {
        if (!Mutation_index[i] || !you.mutation[i])
            continue;

        // this is already handled above:
        if (you.species == SP_NAGA && i == MUT_SPIT_POISON)
            continue;

        j++;
        textcolor( LIGHTGREY );

        if (j > num_lines - 4)
        {
            gotoxy( 1, num_lines - 1 );
            cprintf( MORE_STRING );

            if (getch() == 0)
                getch();

            clrscr();

            // center title
            int x = 40 - strlen(mut_title) / 2;
            if (x < 1)
                x = 1;

            gotoxy(x, 1);
            textcolor(WHITE);
            cprintf(mut_title);
            textcolor(LIGHTGREY);
            gotoxy(1,3);
            j = 1;
        }

        /* mutation is actually a demonic power */
        if (you.demon_pow[i])
        {
            if (you.demon_pow[i] < you.mutation[i])
            {
                textcolor( LIGHTRED );
                cprintf( "+ " );
            }
            else
            {
                textcolor( RED );
                cprintf( "* " );
            }
        }
        else
        {
            cprintf( "  " );
        }

        cprintf( mutation_desc( i ) );
        cprintf(EOL);
    }

    if (j == 0)
        cprintf( "You are not a mutant." EOL );

    if (getch() == 0)
        getch();
#ifdef DOS_TERM
    puttext(1, 1, 80, 25, buffer);
#endif

    //cprintf("xxxxxxxxxxxxx");
    //last_requested = 0;

    return;
}                               // end display_mutations()

static void upgrade_ability_table( int old_abil, int new_abil )
{
    for (int i = 0; i < 52; i++)
    {
        if (you.ability_letter_table[i] == old_abil)
            you.ability_letter_table[i] = new_abil;
    }
}

bool mutate( int which_mutation, bool force, bool failMsg )
{
    int  i;
    int  mutat = which_mutation;

    // Amulets are less effective against the undead decomposition.
    const bool res_mut = ((player_equip( EQ_AMULET, AMU_RESIST_MUTATION )
                            && !one_chance_in( (you.is_undead) ? 3 : 10 ))
                        || you.mutation[MUT_MUTATION_RESISTANCE] > random2(3));

    // Undead bodies don't mutate, they fall apart. -- bwr
    if (you.is_undead)
    {
        if (force || !res_mut)
        {
            mpr( "Your body decomposes!" );

            if (!one_chance_in(3))
                lose_stat( STAT_RANDOM, 1 );
            else
            {
                ouch( 6, 0, KILLED_BY_ROTTING );
                rot_hp( roll_dice(2,3) );
            }

            return (true);
        }

        if (failMsg)
            mpr("You feel odd for a moment.");

        return (false);
    }

    if (!force && res_mut)
    {
        if (failMsg)
            mpr("You feel odd for a moment.");

        return (false);
    }

    if (which_mutation == PICK_RANDOM_MUTATION
        || which_mutation == PICK_NONSTAT_MUTATION)
    {
        const int total_levels = how_mutated();

#if DEBUG_DIAGNOSTICS
        mpr( MSGCH_DIAGNOSTICS, "total mutation levels = %d", total_levels );
#endif

        if (random2(15) < total_levels)
        {
            if (!force && !one_chance_in(3))
                return (false);

            return (delete_mutation( which_mutation ));
        }

        const int scales = get_scale_type();

        for (i = 0; i < 1000; i++)
        {
            mutat = random2( NUM_MUTATIONS );

            if (scales != MUT_NONE && is_scale_mutation( mutat ))
            {
                if (scales == MUT_INTRINSIC)
                    continue;
                else
                    mutat = scales;
            }

            if (random2(10) >= mutation_rarity( mutat ))
                continue;

            if (mutat == MUT_STRONG || mutat == MUT_WEAK
                || mutat == MUT_CLEVER || mutat == MUT_DOPEY
                || mutat == MUT_AGILE || mutat == MUT_CLUMSY)
            {
                if (which_mutation == PICK_NONSTAT_MUTATION)
                    continue;
            }

            const mutation_def &mut = get_mutation_info( mutat );

            if (you.mutation[mutat] < mut.max_levels)
                break;
        }

        // In case the above leaves this set to something bad (or non-existant)
        if (i == 1000 || !mutation_rarity(mutat))
            return (false);
    }

    // These can be forced by demonspawn
    if (is_scale_mutation( mutat ) && body_covered() >= 3 && !force)
        return (false);

    if (mutat == MUT_HORNS && you.species == SP_MINOTAUR)
        return (false);

    // nagas have see invis and res poison and can spit poison
    if (you.species == SP_NAGA
        && (mutat == MUT_ACUTE_VISION || mutat == MUT_POISON_RESISTANCE))
    {
        return (false);
    }

    // gnomes can already sense surroundings
    if (you.species == SP_GNOME && mutat == MUT_MAPPING)
        return (false);

    // spriggans already run at max speed (centaurs can get a bit faster)
    if (you.species == SP_SPRIGGAN && mutat == MUT_FAST)
        return (false);

    // this might have issues if we allowed it -- bwr
    if (you.species == SP_KOBOLD
        && (mutat == MUT_CARNIVOROUS || mutat == MUT_HERBIVOROUS))
    {
        return (false);
    }

    //jmf: added some checks for new mutations
    if (mutat == MUT_STINGER
        && !(you.species == SP_NAGA || player_genus(GENPC_DRACONIAN)))
    {
        return (false);
    }

    if (mutat == MUT_HOOVES
        && (you.species == SP_NAGA || you.species == SP_CENTAUR
            || you.species == SP_KENKU || player_genus(GENPC_DRACONIAN)))
    {
        return (false);
    }

    if (mutat == MUT_BIG_WINGS && !player_genus(GENPC_DRACONIAN))
        return (false);

    const mutation_def &mut = get_mutation_info( mutat );

    if (you.mutation[mutat] >= mut.max_levels)
        return (false);

    // check if opposite mutation exists to cancel
    if (mut.opposite != MUT_NONE)
    {
        if (you.demon_pow[ mut.opposite ])  // don't cancel demon powers
            return (false);

        if (you.mutation[ mut.opposite ])
            return (delete_mutation( mut.opposite, force ));
    }

    //jmf: added some checks for new mutations
    mpr( MSGCH_MUTATION, "You mutate." );

    // apply mutation level
    you.mutation[mutat]++;
    gain_msg( mutat, you.mutation[mutat] - 1 );

    switch (mutat)
    {
    case MUT_STRONG:
        modify_stat( STAT_STRENGTH, 1, true );
        break;

    case MUT_CLEVER:
        modify_stat( STAT_INTELLIGENCE, 1, true );
        break;

    case MUT_AGILE:
        modify_stat( STAT_DEXTERITY, 1, true );
        break;

    case MUT_WEAK:
        modify_stat( STAT_STRENGTH, -1, true );
        break;

    case MUT_DOPEY:
        modify_stat( STAT_INTELLIGENCE, -1, true );
        break;

    case MUT_CLUMSY:
        modify_stat( STAT_DEXTERITY, -1, true );
        break;

    case MUT_STRONG_STIFF:
        modify_stat( STAT_STRENGTH, 1, true );
        modify_stat( STAT_DEXTERITY, -1, true );
        break;

    case MUT_FLEXIBLE_WEAK:
        modify_stat( STAT_STRENGTH, -1, true );
        modify_stat( STAT_DEXTERITY, 1, true );
        break;

    case MUT_HORNS:             // horns force your helmet off
        // horns don't push off soft head gear
        if (you.equip[EQ_HELMET] != -1
            && you.inv[ you.equip[EQ_HELMET] ].sub_type == ARM_CAP)
        {
            break;
        }

        remove_equipment( RMFLAG( EQ_HELMET ) );
        break;

    case MUT_CLAWS:
        // gloves aren't prevented until level three
        if (you.mutation[ mutat ] >= 3 && you.equip[EQ_GLOVES] != -1)
            remove_equipment( RMFLAG( EQ_GLOVES ) );
        break;

    case MUT_HOOVES:
        // boots aren't prevented until level two
        if (you.mutation[ mutat ] >= 2 && you.equip[EQ_BOOTS] != -1)
            remove_equipment( RMFLAG( EQ_BOOTS ) );
        break;

    case MUT_SUMMON_DEMONS:
        if (you.mutation[mutat] == 2)
            upgrade_ability_table( ABIL_SUMMON_MINOR_DEMON, ABIL_SUMMON_DEMONS );
        break;

    case MUT_THROW_FLAMES:
        if (you.mutation[mutat] == 2)
            upgrade_ability_table( ABIL_THROW_FLAME, ABIL_BOLT_OF_FIRE );
        else if (you.mutation[mutat] == 3)
            upgrade_ability_table( ABIL_BOLT_OF_FIRE, ABIL_HELLFIRE );
        break;

    case MUT_THROW_FROST:
        if (you.mutation[mutat] == 2)
            upgrade_ability_table( ABIL_THROW_FROST, ABIL_BOLT_OF_COLD );
        else if (you.mutation[mutat] == 3)
            upgrade_ability_table( ABIL_BOLT_OF_COLD, ABIL_HELLFROST );
        break;

    case MUT_CALL_TORMENT:
        if (you.mutation[mutat] == 2)
            upgrade_ability_table( ABIL_PAIN, ABIL_BOLT_OF_DRAINING );
        break;

    case MUT_SPIT_POISON:
        // breathe poison replaces spit poison (so it takes the slot)
        if (you.mutation[mutat] == 3)
            upgrade_ability_table( ABIL_SPIT_POISON, ABIL_BREATHE_POISON );
        break;

    case MUT_FRAIL:
    case MUT_ROBUST:
        calc_hp_max();
        break;

    default:
        break;
    }

    // find where these things are actually changed
    // -- do not globally force redraw {dlb}
    set_redraw_status( REDRAW_HIT_POINTS );
    set_redraw_status( REDRAW_MAGIC_POINTS );
    set_redraw_status( REDRAW_ARMOUR_CLASS );
    set_redraw_status( REDRAW_EVASION );
    set_redraw_status( REDRAW_EXPERIENCE );
    set_redraw_status( REDRAW_GOLD );

    return (true);
}                               // end mutation()

static int how_mutated(void)
{
    int ret = 0;

    for (int i = 0; i < NUM_MUTATIONS; i++)
    {
        const int levels = you.mutation[i] - you.demon_pow[i];

        if (levels > 0)
        {
            const mutation_def &mut = get_mutation_info(i);

            if (mut.max_levels <= 3)
                ret += levels;
            else
                ret += 1 + levels / (mut.max_levels / 3);
        }
    }

    return (ret);
}                               // end how_mutated()

bool delete_mutation( int which_mutation, bool force, bool need_mutate_msg )
{
    int mutat = which_mutation;

    if (!force && you.mutation[MUT_MUTATION_RESISTANCE] > 1 + random2(2))
    {
        mpr("You feel rather odd for a moment.");
        return (false);
    }

    if (which_mutation == PICK_RANDOM_MUTATION
        || which_mutation == PICK_NONSTAT_MUTATION)
    {
        for (;;)
        {
            mutat = random2( NUM_MUTATIONS );

            if (one_chance_in(1000))
                return (false);

            if (random2(10) >= mutation_rarity(mutat))
                continue;

            if ((which_mutation == PICK_NONSTAT_MUTATION
                    || coinflip())
                && (mutat == MUT_STRONG || mutat == MUT_WEAK
                    || mutat == MUT_CLEVER || mutat == MUT_DOPEY
                    || mutat == MUT_AGILE || mutat == MUT_CLUMSY))
            {
                continue;
            }

            if (you.demon_pow[mutat] < you.mutation[mutat])
                break;
        }
    }

    if (you.mutation[mutat] <= 0)
        return (false);

    if (you.demon_pow[mutat] >= you.mutation[mutat])
        return (false);

    if (need_mutate_msg)
        mpr( MSGCH_MUTATION, "You mutate." );

    you.mutation[mutat]--;
    lose_msg( mutat, you.mutation[mutat] );

    switch (mutat)
    {
    case MUT_STRONG:
        modify_stat( STAT_STRENGTH, -1, true );
        break;

    case MUT_CLEVER:
        modify_stat( STAT_INTELLIGENCE, -1, true );
        break;

    case MUT_AGILE:
        modify_stat( STAT_DEXTERITY, -1, true );
        break;

    case MUT_WEAK:
        modify_stat( STAT_STRENGTH, 1, true );
        break;

    case MUT_DOPEY:
        modify_stat( STAT_INTELLIGENCE, 1, true );
        break;

    case MUT_CLUMSY:
        modify_stat( STAT_DEXTERITY, 1, true );
        break;

    case MUT_STRONG_STIFF:
        modify_stat( STAT_STRENGTH, -1, true );
        modify_stat( STAT_DEXTERITY, 1, true );
        break;

    case MUT_FLEXIBLE_WEAK:
        modify_stat( STAT_STRENGTH, 1, true );
        modify_stat( STAT_DEXTERITY, -1, true );
        break;

    case MUT_FRAIL:
    case MUT_ROBUST:
        calc_hp_max();
        break;

    case MUT_SUMMON_DEMONS:
        if (you.mutation[mutat] == 1)
            upgrade_ability_table( ABIL_SUMMON_DEMONS, ABIL_SUMMON_MINOR_DEMON );
        break;

    case MUT_THROW_FLAMES:
        if (you.mutation[mutat] == 2)
            upgrade_ability_table( ABIL_HELLFIRE, ABIL_BOLT_OF_FIRE );
        else if (you.mutation[mutat] == 1)
            upgrade_ability_table( ABIL_BOLT_OF_FIRE, ABIL_THROW_FLAME );
        break;

    case MUT_THROW_FROST:
        if (you.mutation[mutat] == 2)
            upgrade_ability_table( ABIL_HELLFROST, ABIL_BOLT_OF_COLD );
        else if (you.mutation[mutat] == 1)
            upgrade_ability_table( ABIL_BOLT_OF_COLD, ABIL_THROW_FROST );
        break;

    case MUT_CALL_TORMENT:
        if (you.mutation[mutat] == 2)
            upgrade_ability_table( ABIL_BOLT_OF_DRAINING, ABIL_PAIN );
        break;

    case MUT_SPIT_POISON: // can't be removed yet, but still covered:
        if (you.mutation[mutat] == 3)
            upgrade_ability_table( ABIL_BREATHE_POISON, ABIL_SPIT_POISON );
        break;

    default:
        break;
    }

    // XXX: find where these things are actually altered
    /// -- do not globally force redraw {dlb}
    set_redraw_status( REDRAW_HIT_POINTS );
    set_redraw_status( REDRAW_MAGIC_POINTS );
    set_redraw_status( REDRAW_ARMOUR_CLASS );
    set_redraw_status( REDRAW_EVASION );
    set_redraw_status( REDRAW_EXPERIENCE );
    set_redraw_status( REDRAW_GOLD );

    return (true);
}                               // end delete_mutation()

const char *mutation_desc( int mutat, int level )
{
    static char mut_string[INFO_SIZE];

    // level == -1 means default action of current level
    if (level == -1)
        level = you.mutation[ mutat ];

    const mutation_def &mut = get_mutation_info( mutat );

    if (mutat == MUT_STRONG || mutat == MUT_CLEVER
        || mutat == MUT_AGILE || mutat == MUT_WEAK
        || mutat == MUT_DOPEY || mutat == MUT_CLUMSY)
    {
        snprintf( mut_string, sizeof( mut_string ), "%s%d).",
                  mut.desc[0], level );

        return (mut_string);
    }

    // Some mutations only have one "level", and it's better
    // to show the first level description than a blank description.
    return (mut.desc[level - 1][0] == '\0' ? mut.desc[0] : mut.desc[level - 1]);
}                               // end mutation_desc()

static int get_random_demonspawn_attr( int max_attr, bool only_previous )
{
    int attr = -1;
    int count = 0;

    // This is the point at which we allow 3rd level... this helps guarantee
    // an early grab bag spread, giving a better chance of getting a selection
    // of mutations rather than having only one or two keep advancing.
    const bool low_level = (you.xp_level < 15);

    for (int i = ATTR_DEMONIC_POWER_1; i <= max_attr; i++)
    {
        // skip empty if only looking for pre-existing ones
        if (only_previous && you.attribute[i] == MUT_NONE)
            continue;

        if (you.attribute[i] != MUT_NONE)
        {
            const mutation_def &mut = get_mutation_info( you.attribute[i] );

            // move on to next power if already full
            if (you.demon_pow[ you.attribute[i] ] >= mut.max_levels)
                continue;

            if (low_level && you.demon_pow[ you.attribute[i] ] >= 2)
                continue;
        }

        count++;
        if (one_chance_in( count ))
            attr = i;
    }

    return (attr);
}

/* Use an attribute counter for how many demonic mutations a dspawn has */
void demonspawn( void )
{
    const int  scales    = get_scale_type();
    const bool low_level = (you.xp_level < 10);
    const int  max_attr  = (low_level) ? ATTR_DEMONIC_POWER_2
                                       : ATTR_DEMONIC_POWER_5;

    int attr = get_random_demonspawn_attr( max_attr, false );

    ASSERT( attr != -1 );  // should never fail the "false" case

    mpr( MSGCH_INTRINSIC_GAIN, "Your demonic ancestry asserts itself..."  );

    int mutat = you.attribute[ attr ];
    int levels = 1;

    if (mutat == MUT_NONE)
    {
        // Merged the demonspawn lists into a single loop.  Now a high level
        // character can potentially get mutations from the low level list if
        // its having trouble with the high level list.
        int counter = 0;

        do
        {
            // Making sure these are both clean to start each iteration.
            // Also note that given the nature of this clobber-through,
            // if you set "levels" you need to make sure that you also
            // set it in every case after that one, right down to the
            // bottom of the loop... which is the only place it's being
            // set right now.  It's essentially there so that if a
            // high level character gets an extra low level mutation,
            // they get an extra level of it (since things like scales
            // only really shine if you have a chance of getting to three
            // levels).
            mutat = MUT_NONE;
            levels = 1;

            if (!low_level)
            {
                if (you.skills[SK_CONJURATIONS] < 5
                    && you.skills[SK_NECROMANCY] < 5
                    && !you.mutation[MUT_TORMENT_RESISTANCE])
                {
                    mutat = MUT_CALL_TORMENT;
                }

                if (you.skills[SK_SUMMONINGS] < 5
                    && you.religion != GOD_MAKHLEB
                    && one_chance_in(3))
                {
                    mutat = MUT_SUMMON_DEMONS;
                }

                if (one_chance_in(5))
                    mutat = MUT_MAGIC_RESISTANCE;

                if (one_chance_in(6))
                    mutat = MUT_TELEPORT_AT_WILL;

                if (one_chance_in(8))
                    mutat = MUT_REGENERATION;

                if (!you.mutation[MUT_CALL_TORMENT] && one_chance_in(15))
                    mutat = MUT_TORMENT_RESISTANCE;

                if (one_chance_in(10))
                    mutat = MUT_NEGATIVE_ENERGY_RESISTANCE;

                if (one_chance_in(8))
                    mutat = MUT_MAPPING;

                if (one_chance_in(10))
                    mutat = MUT_TELEPORT_CONTROL;

                if (you.skills[SK_TRANSLOCATIONS] < 5 && one_chance_in(12))
                    mutat = MUT_PANDEMONIUM;

                if (you.religion != GOD_VEHUMET && one_chance_in(10))
                    mutat = MUT_DEATH_STRENGTH;

                if (you.religion != GOD_VEHUMET && one_chance_in(10))
                    mutat = MUT_CHANNEL_HELL;

                if (you.skills[SK_NECROMANCY] < 5
                    && you.religion != GOD_YREDELEMNUL
                    && one_chance_in(8))
                {
                    mutat = MUT_RAISE_DEAD;
                }

                if (you.skills[SK_UNARMED_COMBAT] > 5 && one_chance_in(14))
                    mutat = MUT_DRAIN_LIFE;
            }

            // check here so we can see if we need to extend our options:
            if (mutat != MUT_NONE && you.demon_pow[mutat] > 0)
                mutat = MUT_NONE;

            if (low_level || (counter > 1 && mutat == MUT_NONE))
            {
                if (!you.mutation[MUT_THROW_FROST]         // only one of these
                    && !you.mutation[MUT_THROW_FLAMES]
                    && !you.mutation[MUT_BREATHE_FLAMES]
                    && !you.mutation[MUT_SPIT_POISON]
                    && one_chance_in( (low_level) ? 5 : 20 ))
                {
                    // try to give the flavour the character doesn't have:
                    if (!you.skills[SK_FIRE_MAGIC])
                    {
                        mutat = one_chance_in(5) ? MUT_BREATHE_FLAMES
                                                 : MUT_THROW_FLAMES;
                    }
                    else if (!you.skills[SK_ICE_MAGIC])
                        mutat = MUT_THROW_FROST;
                    else if (!you.skills[SK_POISON_MAGIC] && one_chance_in(7))
                        mutat = MUT_SPIT_POISON;
                    else if (!you.skills[SK_CONJURATIONS] || one_chance_in(5))
                        mutat = (coinflip() ? MUT_THROW_FLAMES : MUT_THROW_FROST);
                }

                // XXX: Really we should be reducing the number of checks
                // against skills list (it's hacky, a poor solution, and
                // prone to possible twinking).  That's not to say that we
                // should be giving them something they already have,
                // but that the skill should probably be applied to the
                // mutation in some synergistic way.  For example, summoning
                // plus this mutation might give random free autosummons when
                // the PC is badly injured... or perhaps it could be the
                // other way and the mutation could improve summoning spells
                // (ie more demons, longer (sometimes permanent) duration,
                // greater demons that aren't just charmed).  The player
                // shouldn't really have any control over what random
                // abilities they get, and should be happy with whatever
                // they get.
                if (you.skills[SK_SUMMONINGS] < 5
                    && you.skills[SK_NECROMANCY] < 5
                    && one_chance_in(3))
                {
                    mutat = MUT_SUMMON_DEMONS;
                }

                if (you.skills[SK_NECROMANCY] < 5
                    && !you.mutation[MUT_TORMENT_RESISTANCE]
                    && one_chance_in(10))
                {
                    mutat = MUT_CALL_TORMENT;
                }

                if (you.skills[SK_UNARMED_COMBAT] >= 3 && one_chance_in(20))
                    mutat = MUT_DRAIN_LIFE;

                if (one_chance_in(4))
                    mutat = MUT_POISON_RESISTANCE;

                if (one_chance_in(5))
                    mutat = MUT_ACUTE_VISION;

                if (you.skills[SK_TRANSLOCATIONS] < 5 && one_chance_in(12))
                    mutat = MUT_BLINK;

                if (one_chance_in(25))
                    mutat = MUT_REPULSION_FIELD;

                // Note: in testing the counter hardly got anywhere near 100
                // (hardly got past 2 for that matter)...  but should we ever
                // get in that much trouble, we're going to open up the gates
                // for the cosmetic mutations since those should never have any
                // type of skill/mutation//god conficts.
                if (!you.demon_pow[ MUT_HORNS ]
                    && !you.demon_pow[ MUT_CLAWS ]
                    && !you.demon_pow[ MUT_HOOVES ]
                    && (one_chance_in( (low_level) ? 5 : 30 )
                        || (mutat == MUT_NONE && counter > 100)))
                {
                    // If we have a temporary cosmetic mutation, it becomes
                    // permanent... else we pick a random one.
                    if (you.mutation[ MUT_HORNS ])
                        mutat = MUT_HORNS;
                    else if (you.mutation[ MUT_CLAWS ])
                        mutat = MUT_CLAWS;
                    else if (you.mutation[ MUT_HOOVES ])
                        mutat = MUT_HOOVES;
                    else
                    {
                        mutat = coinflip() ? MUT_HORNS : MUT_CLAWS;

                        if (one_chance_in(10))
                            mutat = MUT_HOOVES;
                    }
                }

                if ((scales == MUT_NONE
                        || !you.demon_pow[ scales ])
                    && (one_chance_in( (low_level) ? 3 : 10 )
                        || (mutat == MUT_NONE && counter > 100)))
                {
                    // if we have temporary scales, choose them first
                    if (scales != MUT_NONE)
                        mutat = scales;
                    else
                        mutat = pick_random_scale_type();
                }

                if (!low_level && mutat != MUT_NONE)
                    levels = 2;
            }

            if (mutat != MUT_NONE && you.demon_pow[mutat] > 0)
                mutat = MUT_NONE;

            counter++;
        }
        while (mutat == MUT_NONE && counter < 5000);
    }

    // These cases occur in the !low_level case as well as the
    // low_level case, and thus might have been set without
    // the level boost that the lower version gets.  We modify
    // levels here so that we don't have to worry about it
    // in all the cases in the loop (see note at top).
    if (!low_level
        && (mutat == MUT_CALL_TORMENT
            || mutat == MUT_THROW_FLAMES
            || mutat == MUT_SUMMON_DEMONS
            || mutat == MUT_DRAIN_LIFE))
    {
        levels = 2;
    }

    if (mutat != MUT_NONE && demon_mutate( mutat, levels ))
        you.attribute[ attr ] = mutat;
    else
    {
        // Okay, either we failed to find a good new mutation, or
        // we failed to increase the one we picked.  So let's give
        // another try by picking a previous mutation to boost.
        attr = get_random_demonspawn_attr( max_attr, true );

        if (attr == -1 || !demon_mutate( you.attribute[attr], 1 ))
        {
            // leaving the old cop-out, but it really shouldn't happen
            modify_stat( STAT_STRENGTH, 1, true );
            modify_stat( STAT_INTELLIGENCE, 1, true );
            modify_stat( STAT_DEXTERITY, 1, true );
            mpr( MSGCH_INTRINSIC_GAIN, "You feel much better now."  );
        }
    }
}                               // end demonspawn()

static bool demon_mutate( int which_mut, int levels )
{
    bool ret = false;

    const mutation_def &mut = get_mutation_info( which_mut );

    if (you.demon_pow[ which_mut ] >= mut.max_levels)
        return (false);

    const int diff = you.mutation[ which_mut ] - you.demon_pow[ which_mut ];

    if (diff > 0)
    {
        mpr( MSGCH_MUTATION, "Your %s mutation feels more natural.", mut.name );

        if (diff >= levels)
            you.demon_pow[ which_mut ] += levels;
        else
        {
            you.demon_pow[ which_mut ] += diff;
            levels -= diff;  // still need to mutate a bit more below
        }

        ret = true;
    }


    if (levels)
    {
        const int opp = mut.opposite;

        // clear opposite mutation first (doesn't eat levels)
        if (opp != MUT_NONE && you.mutation[opp] > 0)
        {
            while (delete_mutation( opp, true, false ))
                ;
        }

        // try to apply levels
        for (int i = 0 ; i < levels; i++)
        {
            if (mutate( which_mut, true ))
            {
                you.demon_pow[ which_mut ]++;
                ret = true;
            }
        }
    }

    return (ret);
}                               // end demon_mutate()

mutation_type give_good_mutation( bool failMsg )
{
    const int temp_rand = random2(25);

    const mutation_type which = ((temp_rand == 24) ? MUT_STRONG :
                                 (temp_rand == 23) ? MUT_CLEVER :
                                 (temp_rand == 22) ? MUT_AGILE :
                                 (temp_rand == 21) ? MUT_HEAT_RESISTANCE :
                                 (temp_rand == 20) ? MUT_COLD_RESISTANCE :
                                 (temp_rand == 19) ? MUT_SHOCK_RESISTANCE :
                                 (temp_rand == 18) ? MUT_REGENERATION :
                                 (temp_rand == 17) ? MUT_TELEPORT_CONTROL :
                                 (temp_rand == 16) ? MUT_MAGIC_RESISTANCE :
                                 (temp_rand == 15) ? MUT_FAST :
                                 (temp_rand == 14) ? MUT_ACUTE_VISION :
                                 (temp_rand == 13) ? MUT_REPULSION_FIELD :
                                 (temp_rand == 12) ? MUT_POISON_RESISTANCE :
                                 (temp_rand == 11) ? MUT_TELEPORT_AT_WILL :
                                 (temp_rand == 10) ? MUT_SPIT_POISON :
                                 (temp_rand ==  9) ? MUT_MAPPING :
                                 (temp_rand ==  8) ? MUT_BREATHE_FLAMES :
                                 (temp_rand ==  7) ? MUT_BLINK :
                                 (temp_rand ==  6) ? MUT_CLARITY :
                                 (temp_rand ==  5) ? MUT_ROBUST
                                                   : pick_random_scale_type());

    return (mutate( which, false, failMsg ) ? which : MUT_NONE );
}                               // end give_good_mutation()

mutation_type give_bad_mutation( bool forceMutation, bool failMsg )
{
    const int temp_rand = random2(11);

    const mutation_type which = ((temp_rand >= 10) ? MUT_HERBIVOROUS :
                                 (temp_rand ==  9) ? MUT_FAST_METABOLISM :
                                 (temp_rand ==  8) ? MUT_WEAK :
                                 (temp_rand ==  7) ? MUT_DOPEY :
                                 (temp_rand ==  6) ? MUT_CLUMSY :
                                 (temp_rand ==  5) ? MUT_TELEPORT :
                                 (temp_rand ==  4) ? MUT_DEFORMED :
                                 (temp_rand ==  3) ? MUT_LOST :
                                 (temp_rand ==  2) ? MUT_DETERIORATION :
                                 (temp_rand ==  1) ? MUT_BLURRY_VISION
                                                   : MUT_FRAIL);

    return (mutate( which, forceMutation, failMsg) ? which : MUT_NONE);
}                               // end give_bad_mutation()

//jmf: might be useful somewhere (eg Xom or transmigration effect)
mutation_type give_cosmetic_mutation( void )
{
    mutation_type mutation = MUT_NONE;
    int counter = 0;

    do
    {
        mutation = MUT_DEFORMED;

        if (one_chance_in(6))
            mutation = MUT_ROBUST;

        if (one_chance_in(6))
            mutation = MUT_FRAIL;

        if (one_chance_in(5))
            mutation = MUT_TOUGH_SKIN;

        if (one_chance_in(4))
            mutation = MUT_CLAWS;

        if (you.species != SP_CENTAUR && you.species != SP_NAGA
            && you.species != SP_KENKU && !player_genus(GENPC_DRACONIAN)
            && one_chance_in(5))
        {
            mutation = MUT_HOOVES;
        }

        if (player_genus(GENPC_DRACONIAN) && one_chance_in(5))
            mutation = MUT_BIG_WINGS;

        if (one_chance_in(5))
            mutation = MUT_CARNIVOROUS;

        if (one_chance_in(6))
            mutation = MUT_HORNS;

        if ((you.species == SP_NAGA || player_genus( GENPC_DRACONIAN ))
            && one_chance_in(4))
        {
            mutation = MUT_STINGER;
        }

        if (!player_genus(GENPC_DRACONIAN) && one_chance_in(7))
            mutation = MUT_SPIT_POISON;

        if (!(you.species == SP_NAGA || player_genus(GENPC_DRACONIAN))
            && one_chance_in(8))
        {
            mutation = MUT_BREATHE_FLAMES;
        }

        const mutation_def &mut = get_mutation_info( mutation );

        if (you.mutation[mutation] >= mut.max_levels)
            mutation = MUT_NONE;
    }
    while (mutation == MUT_NONE && counter++ < 5000);

    if (mutation != MUT_NONE && mutate( mutation ))
        return (mutation);

    return (MUT_NONE);
}                               // end give_cosmetic_mutation()

#include "config.h"

//#include "defines.h"
/*#define MLAVA0 220
#define MLAVA1 221
#define MLAVA2 222
#define MLAVA3 223
#define MLAVA4 224
#define MLAVA5 225
#define MWATER0 230
#define MWATER1 231
#define MWATER2 232
#define MWATER3 233
#define MWATER4 234
#define MWATER5 235
*/
#ifdef DOS
#include <conio.h>
#endif

#include "externs.h"
#include "enum.h"

#include <stdlib.h>
#include <string.h>
#include "itemname.h"
#include "macro.h"
#include "mstruct.h"
#include "player.h"
#include "randart.h"
#include "skills2.h"
#include "stuff.h"

void print_description(char descr [1000]);
void print_ench(char descpr [1000], unsigned char item_plus);
void append_value(char descpr [1000], int valu, char plussed);
void randart_descpr(char descpr [1000], int item_class, int item_type, int item_plus, int item_plus2, int item_dam);
void describe_demon(char descpr [1000]);

/*
Contains sketchy descriptions of every monster in the game.
*/
void describe_monsters(int class_described, unsigned char which_mons)
{

   unsigned char useless = which_mons;
   which_mons = useless;

   char descpr [1000];
#ifdef DOS_TERM
   char buffer[3400];
   gettext(25, 1, 80, 25, buffer);
#endif

//   strcpy(st_pass, "");

#ifdef DOS_TERM
   window(25, 1, 80, 25);
#endif

   clrscr();

   strcpy(descpr, "");

switch(class_described)
{

case 0: // giant ant
strcpy(descpr, "A black ant with poisonous pincers, about the size of a large dog.");
break;

case 1: // giant bat
strcpy(descpr, "A huge black bat.");
break;

case 2: // centaur
strcpy(descpr, "A hybrid with the torso of a human atop the body of a large horse.");
break;

case 3: // red devil
strcpy(descpr, "The Red Devil is slightly shorter than a human, \
but muscular and covered in spikes and horns. Two short wings sprout from its shoulders.");
break;

case 4: // ?
strcpy(descpr, "");
break;

case 5: // fungus
strcpy(descpr, "A lumpy grey fungus, growing well in the dank underground dungeon.");
break;

case 6: // goblin
strcpy(descpr, "A race of short, ugly humanoids.");
break;

case 7: // hound
strcpy(descpr, "A fearsome hunting dog.");
break;

case 8: // imp
strcpy(descpr, "An ugly little minor demon.");
break;

case 9: // jackal
strcpy(descpr, "A canine scavenger. Packs of these creatures roam the underworld, searching for carrion to devour.");
break;

case 10: // k
strcpy(descpr, "A giant bee, bearing a deadly sting which can sting repeatedly.");
break;

case 11: // ?
strcpy(descpr, "");
break;

case 12: // manticore
strcpy(descpr, "A hideous cross-breed, bearing the features of a human and a lion, with great bat-like wings. Its tail bristles with spikes, \
which can be loosed at potential prey.");
break;

case 13: // necrophage
strcpy(descpr, "A vile undead creation of the most unholy necromancy, these creatures are made from the decaying corpses of humanoid creatures. \
They exist to spread disease and decay, and gain power from the decaying corpses of other beings.");
break;

case 14: // orc
strcpy(descpr, "An ugly subterranean race, orcs combine the worst features of humans, pigs, and several other unpleasant creatures put together.");
break;

case 15: // phantom
strcpy(descpr, "A certain type of undead spirit.");
break;

case 16: // q
strcpy(descpr, "");
break;

case 17: // rat
strcpy(descpr, "Rats who have grown large and aggressive in the pestilential dungeon environment.");
break;

case 18: // scorpion
strcpy(descpr, "A giant black scorpion, its body covered in thick armour plating, and its tail tipped by a nasty venomous sting.");
break;

case 56:
case 19: // tunnelling worm
strcpy(descpr, "A gargantuan worm, its huge maw capable of crushing rock into dust with little trouble.");
break;

case 20: // u
strcpy(descpr, "An ugly thing. Yuck.");
break;

case 21: // vortex
strcpy(descpr, "A swirling cloud of flame.");
break;

case 22: // w
strcpy(descpr, "A giant worm, with unusually large teeth (for a worm).");
break;

case 23: // x
strcpy(descpr, "A hideous form, created or summoned by some arcane process.");
break;

case 24: // y
strcpy(descpr, "A giant wasp covered with thick plates of yellow chitinous armour.");
break;

case 25: // z
strcpy(descpr, "A corpse raised from the dead to undeath by necromancy.");
break;

case 26: // A
strcpy(descpr, "");
break;

case 27: // B
strcpy(descpr, "A huge black beetle with great crunching mandibles and an armour-plated body.");
break;

case 28: // C
strcpy(descpr, "A giant with one eye in the centre of its forehead. Despite their lack of binocular vision, cyclopses throw boulders with fearsomely accuracy.");
break;

case 29: // D
strcpy(descpr, "A great reptilian beast, covered in thick green scales and with two huge bat-like wings. Little trails of smoke spill from its toothy maw.");
break;

case 30: // E
strcpy(descpr, "A huge ogre with two heads on top of a bloated ogre body. It is capable of holding a weapon in each giant hand.");
break;

case 31: // F
strcpy(descpr, "One of the most fearsome denizens of Hell. A huge and powerful demon wreathed in hellfire, with great scaly wings.");
break;

case 32: // G
strcpy(descpr, "A volatile floating ball of spores, covered in knobbly rhizome growths.");
break;

case 33: // g
strcpy(descpr, "Larger relatives of goblins.");
break;

case 34: // I
strcpy(descpr, "A terrible creature, formed of snow and crystalline ice. Its feet leave puddles of icy water on the floor.");
break;

case 35: // J
strcpy(descpr, "A pulsating mass of acidic protoplasm. It can and will eat almost anything, and grows a little each time...");
break;

case 36: // K
strcpy(descpr, "Reputedly the creation of an ancient demon-god, kobolds are small goblin-like creatures with canine heads.");
break;

case 37: // L
strcpy(descpr, "A wizard who didn't want to die, a Lich is a skeleton or decaying corpse kept alive by a mighty exercise of necromancy. These undead creatures can wield great magic and are best avoided by all but the most confident (or stupid) adventurers.");
break;

case 38: // M
strcpy(descpr, "An undead figure covered in bandages and embalming fluids, compelled to walk by an ancient curse. It radiates a malign aura to those who intrude on its domain.");
break;

case 39: // N
strcpy(descpr, "A serpent with the head of a human. These nagas are often used as guardians by powerful creatures.");
break;

case 40: // O
strcpy(descpr, "A larger, uglier and usually fatter relative of orcs and goblins.");
break;

case 41: // P
strcpy(descpr, "Few plants can grow in the unpleasant dungeon environment, but some have managed to adapt and even thrive underground in the absence of the sun.");
break;

case 42: // Q
strcpy(descpr, "Even larger and more dangerous-looking than its offspring, this creature wants you out of its hive. Now!");
break;

case 78: // fake R
case 43: // R
strcpy(descpr, "A type of demon who comes to the material world in search of power and knowledge. Rakshasas are experts in the art of illusion, among other things.");
break;

case 44: // S
strcpy(descpr, "The common dungeon snake.");
break;

case 45: // T
strcpy(descpr, "A huge, nasty-looking creature. Its thick and knobbly hide seems to heal almost instantly from most wounds.");
break;

case 46: // U
strcpy(descpr, "These creatures are usually unseen by the eyes of most, and those few who have seen them would rather not have.");
break;

case 47: // V
strcpy(descpr, "A powerful undead. It wants to drink your blood! ");
break;

case 48: // W
strcpy(descpr, "This undead spirit appears as a cloud of black mist surrounding an insubstantial skeletal form. Its eyes burn bright with unholy malevolence.");
break;

case 49: // X
strcpy(descpr, "A huge and hideous creature, created or summoned by some arcane process.");
break;

case 50: // Y
strcpy(descpr, "The common dungeon yak. Covered in shaggy yak hair and bearing a nasty pair of yak horns.");
break;

case 51: // Z
strcpy(descpr, "A corpse raised from the dead to undeath by necromancy.");
break;

case 52: // o war
strcpy(descpr, "An armoured orc, obviously experienced in the ways of hacking other creatures apart.");
break;

case 53: // K dem
strcpy(descpr, "A kobold who has learned to summon and direct demons.");
break;

case 54: // o wiz
strcpy(descpr, "While orcs are generally quite stupid, occasionally one of them develops an aptitude for magic.");
break;

case 55: // o knight
strcpy(descpr, "A heavily armoured orc, covered in scars from many past battles.");
break;

//case 56: // worm tail
//strcpy(descpr, "");
//break;

case 57: // wyvern
strcpy(descpr, "Although smaller and less formidable than true dragons, wyverns are nonetheless a foe to be reckoned with.");
break;

case 58: // big kobold
strcpy(descpr, "A kobold of unusual size.");
break;

case 59: // giant eyeball
strcpy(descpr, "A giant eyeball, with a captivating stare.");
break;

case 60: // wight
strcpy(descpr, "An ancient warrior, kept in a state of undeath by its will to live.");
break;

case 61: // oklob plant
strcpy(descpr, "A vicious plant, dripping with vitriol.");
break;

case 62: // wolf s
strcpy(descpr, "A large hairy spider with vicious mandibles, roaming the dungeon in search of food.");
break;

case 63: // shadow
strcpy(descpr, "A walking undead shadow.");
break;

case 64: // hungry ghost
strcpy(descpr, "The undead form of someone who died of starvation, this creature wants the same thing to happen to you!");
break;

case 65: // eye of draining
strcpy(descpr, "These hovering eyeballs are especially loathed by wizards.");
break;

case 66: // butterfly
strcpy(descpr, "A large multicoloured butterfly with beautifully patterned wings.");
break;

case 67: // wandering mushroom
strcpy(descpr, "A large, fat mushroom.");
break;

case 68: // Efreet
strcpy(descpr, "A huge and muscular figure engulfed in a cloud of searing flame.");
break;

case 69: // brain worm
strcpy(descpr, "A slimy mauve worm with a greatly distended head.");
break;

case 70: // Brain
strcpy(descpr, "A huge wrinkled brain, floating just off the floor. Every now and then it seems to pulsate.");
break;

case 71: // B B
strcpy(descpr, "A huge beetle with an almost impenetrable rocky carapace.");
break;

case 72: // flying skull
strcpy(descpr, "Unholy magic keeps a disembodied undead skull hovering above the floor. It has a nasty set of teeth.");
break;

case 73: // h hound
strcpy(descpr, "A great black dog, with smoke pouring from its fanged mouth.");
break;

case 74: // minotaur
strcpy(descpr, "A large human with the head of a bull. It makes its home in secluded labyrinths.");
break;

case 75: // ice dragon
strcpy(descpr, "Like a normal dragon, only white and covered in frost.");
break;

case 76: // Slime creature
strcpy(descpr, "An icky glob of slime, which slithers along the ground.");
break;

case 77: // freezing W
strcpy(descpr, "A cloud of freezing air surrounding an incorporeal skeletal form.");
break;

// 78 is fake R

case 79: // GooE
strcpy(descpr, "A levitating ball, covered in malignant eyes.");
break;

case 80: // hellion
strcpy(descpr, "A frightful demon, covered in roaring hellfire.");
break;

case 81: // rot dev
strcpy(descpr, "A hideous decaying form.");
break;

case 82: // torm
strcpy(descpr, "This cruel and malicious devil is covered in all manner of claws, spines and hooks.");
break;

case 83: // reaper
strcpy(descpr, "A skeletal form wielding a giant scythe. It has come for your soul!");
break;

case 84: // soul eater
strcpy(descpr, "This greater demon looks like a shadow gliding through the air towards you. It radiates an intense aura of negative power.");
break;

case 85: // hairy
strcpy(descpr, "A demon covered in hair. Watch out - it may have fleas!");
break;

case 86: // ice devil
strcpy(descpr, "A demon covered in glittering ice.");
break;

case 87: // blue devil
strcpy(descpr, "A nasty blue thing. It looks cold.");
break;

case 88: // beast
strcpy(descpr, "A weird and hideous cross between beast and human.");
break;

case 89: // iron devil
strcpy(descpr, "A hideous humanoid figure with metal skin.");
break;


case 98: // glowing (unstable) shapeshifter
strcpy(descpr, "A shapeshifter who has lost control over his, her or its transformations, and is constantly changing form.");
break;

case 99: // shapeshifter
strcpy(descpr, "A weird creature with the power to change its form. It is very rarely observed in its natural state.");
break;

case 100: // giant mite
strcpy(descpr, "A large arachnid with vicious poisoned mouth-parts.");
break;

case 101: // steam dragon
strcpy(descpr, "A relatively small grey dragon, with steam pouring from its mouth.");
break;

case 102: // v u
strcpy(descpr, "A very ugly thing. Double yuck.");
break;

case 103: //
strcpy(descpr, "An orc wizard who draws power from Hell.");
break;

case 104: // H
strcpy(descpr, "A large creature with the hindquarters of a horse and the wings, talons and head of an eagle.");
break;

case 105: //
strcpy(descpr, "A huge creature with the hindquarters of a lion and the wings, head and talons of a great eagle.");
break;

case 106: // hydra
strcpy(descpr, "A great reptilian beast, distantly related to the dragon. It has many heads, and the potential to grow many more!");
break;

case 107: // skeletons
case 108:
strcpy(descpr, "A skeleton compelled to unlife by the exercise of necromancy.");
break;

case 109: // hell knight
strcpy(descpr, "A heavily armoured warrior, in league with the powers of Hell.");
break;

case 110: // necromancer
strcpy(descpr, "A wizard who specialises in the practice of necromantic magic.");
break;

case 111: // wizard
strcpy(descpr, "An evil wizard.");
break;

case 112: // orc priest
strcpy(descpr, "A servant of the Gods of the orcs.");
break;

case 113: // orc high priest
strcpy(descpr, "An exalted servant of the Gods of the orcs");
break;

case 114: // human
strcpy(descpr, "");
break;

case 115: // gnoll
strcpy(descpr, "A large relative of orcs and goblins.");
break;

case 116: // clay golem
strcpy(descpr, "A huge animated clay statue.");
break;

case 117: // wood golem
strcpy(descpr, "An animated wooden statue.");
break;

case 118: // stone golem
strcpy(descpr, "A huge animated stone statue.");
break;

case 119: // iron golem
strcpy(descpr, "A huge animated metal statue.");
break;

case 120: // crystal golem
strcpy(descpr, "A huge animated crystal statue.");
break;

case 121: // toenail golem
strcpy(descpr, "A huge animated statue made entirely from toenail clippings. Some people just have too much time on their hands.");
break;

case 122: // mottled dragon
strcpy(descpr, "A small dragon with strangely mottled scales.");
break;

case 123: // earth elemental
strcpy(descpr, "A spirit drawn from the elemental plane of earth, which exists in this world by inhabiting a lump of earth and rocks.");
break;

case 124: // fire elemental
strcpy(descpr, "A spirit drawn from the elemental plane of fire, which exists in this world as a brilliant column of raging flames.");
break;

case 125: // air elemental
strcpy(descpr, "A spirit drawn from the elemental plane of air. It exists in this world as a swirling vortex of air, which is often undetectable without magical means.");
break;

case 126: // Ice fiend
strcpy(descpr, "One of the most terrible denizens of the many Hells, the Ice Fiend is a huge icy figure, covered in frost and wreathed in freezing air.");
break;

case 127: // shadow fiend
strcpy(descpr, "One of the most terrible denizens of the many Hells, this horrible being appears as a great mass of writhing shadows which occasionally reveal the skeletal form of a huge demon.");
break;

case 128: // brown snake
strcpy(descpr, "A large brown snake.");
break;

case 129: // giant lizard
strcpy(descpr, "A huge lizard, with great crunching jaws.");
break;

case 130: // Spectre
strcpy(descpr, "A hideous translucent green undead spirit.");
break;

case 131: // pulsating mass
strcpy(descpr, "A revolting glob of writhing flesh.");
break;

case 132: // storm D
strcpy(descpr, "A huge and very powerful dragon. Sparks crackle along its enormous scaly wings.");
break;

case 133: // yaktaur
strcpy(descpr, "Like a centaur, but half yak.");
break;

case 134: // death yak
strcpy(descpr, "A larger and beefier relative of the common dungeon yak. Its little red eyes gleam with hunger for living flesh.");
break;

case 135: // rock troll
strcpy(descpr, "An enormous and very nasty-looking creature. Its rocky hide seems to heal almost instantaneously from most wounds.");
break;

case 136: // stone giant
strcpy(descpr, "A gigantic humanoid figure, with grey skin almost as hard as rock. It carries a supply of boulders, and wants to play catch with you.");
break;

case 137: // flayed ghost
strcpy(descpr, "A hideous undead creature, with torn skin hanging from an emaciated body.");
break;

case 138: // bumblebee
strcpy(descpr, "A very large and fat hairy bee.");
break;

case 139: // redback
strcpy(descpr, "A vicious black spider with a splash of red on its swollen abdomen. Its mandibles drip with lethal poison.");
break;

case 140: // wisp
strcpy(descpr, "A thin wisp of floating gas.");
break;

case 141: // vapour
strcpy(descpr, "A normally invisible cloud of weird-looking vapour.");
break;

case 142: // ogre mage
strcpy(descpr, "An rare ogre, skilled in the use of magic.");
break;

case 143: // spiny worm
strcpy(descpr, "A great black worm, its many-segmented body covered in spikey plates of chitinous armour. Acidic venom drips from its toothy maw.");
break;

case 144: // dancing weapon
strcpy(descpr, "A weapon dancing in the air. ");
break;

case 145: // titan
strcpy(descpr, "A huge and very powerful giant. ");
break;

case 146: // golden dragon
strcpy(descpr, "A great dragon covered in shining golden scales. ");
break;

// 147 is elf dummy monster

case 148: // lindworm
strcpy(descpr, "A very large and very fat lizard. ");
break;

case 149: // elephant slug
strcpy(descpr, "A huge grey slug with folds of wrinkled skin. ");
break;

case 150: // war dog
strcpy(descpr, "A vicious dog, trained to kill. ");
break;

case 151: // grey rat
strcpy(descpr, "A very large grey rat. ");
break;

case 152: // green rat
strcpy(descpr, "A very large rat, with hair and skin of a most peculiar green colour. ");
break;

case 153: // orange rat
strcpy(descpr, "A huge rat, with weird knobbly orange skin. It glows with unholy energies. ");
break;

case 154: // black snake
strcpy(descpr, "A large black snake. ");
break;

case 155: // sheep
strcpy(descpr, "A stupid woolly animal, with murder in its eyes. ");
break;

case 156: // ghoul
strcpy(descpr, "An undead creature created by a similar curse to that used in the production of the necrophage. ");
break;

case 157: // hog
strcpy(descpr, "A large, fat and very ugly pig. ");
break;

case 158: // mosquito
strcpy(descpr, "A huge, bloated mosquito. It looks diseased.");
break;

case 159: // giant centipede
strcpy(descpr, "It has a lot of legs.");
break;

case 160: // iron troll
strcpy(descpr, "A great troll covered in armour plates of rusty iron. ");
break;

case 161: // Naga
strcpy(descpr, "A weird hybrid; human from the waist up, it has a snake tail instead of legs.");
break;

case 162: // Fire giant
strcpy(descpr, "A red giant, breathing clouds of smoke.");
break;

case 163: // Frost giant
strcpy(descpr, "A frozen giant.");
break;

case 164: // Firedrake
strcpy(descpr, "A small dragon, puffing clouds of smoke.");
break;

case 165: // Shadow dragon
strcpy(descpr, "A great shadowy shape, radiating evil and death.");
break;

case 166: // yellow snake
strcpy(descpr, "A large yellow tubular reptile.");
break;

case 167: // grey snake
strcpy(descpr, "A very large grey python.");
break;

case 168: // deep troll
strcpy(descpr, "A stooped troll.");
break;

case 169: // giant blowfly
strcpy(descpr, "A huge and irritating fly.");
break;

case 170: // red wasp
strcpy(descpr, "A huge wasp with a viciously barbed stinger.");
break;

case 171: // swamp dragon
strcpy(descpr, "A slimy dragon, covered in swamp muck. Poisonous gasses dribble from its snout.");
break;

case 172: // swamp drake
strcpy(descpr, "A small and slimy dragon, covered in swamp muck. It smells horrible.");
break;

case 173: // soldier ant
strcpy(descpr, "A giant ant with large mandibles and a vicious sting.");
break;

case 174: // hill giant
strcpy(descpr, "Although one of the smaller giant varieties, the hill giant is still big enough to be dangerous.");
break;

case 175: // Queen ant
strcpy(descpr, "A bloated insect, covered in thick chitinous armour. Now you know where all those ants keep coming from!");
break;

case 176: // ant larva
strcpy(descpr, "A baby ant. Isn't it cute?");
break;

case 177: // giant frog
strcpy(descpr, "It probably didn't get this big by eating little insects.");
break;

case 178: // giant brown frog
strcpy(descpr, "A very large and vicious-looking carnivorous frog. Its knobbly brown skin blends in with the rough rock of your surroundings.");
break;

case 179: // spiny frog
strcpy(descpr, "Although slightly smaller than its cousin, the giant brown frog, the spiny frog makes up for lack of size by being covered in wickedly barbed spines and spurs.");
break;

case 180: // blink frog
strcpy(descpr, "A weird-looking frog, constantly blinking in and out of reality.");
break;

case 240: // Shadow wraith
strcpy(descpr, "A mist-wreathed skeletal shadow hanging in mid-air, this creature is almost invisible even to your enhanced sight. "); // assumes: to read this message, has see invis
break;

case 241: // Giant amoeba
strcpy(descpr, "A pulsating lump of protoplasm. ");
break;

case 242: // Giant Slug
strcpy(descpr, "A huge and disgusting gastropod. ");
break;

case 243: // Giant Snail
strcpy(descpr, "A huge and disgusting shelled gastropod. ");
break;

case 244: // spatial vortex
strcpy(descpr, "A shimmering, twisted coil of space.");
break;

case 245: // Pit Fiend
strcpy(descpr, "A huge winged fiend with incredibly tough skin.");
break;

case 246: // boring beetle
strcpy(descpr, "A large brown beetle with huge, rock-crushing mandibles.");
break;

case 247: // gargoyle
strcpy(descpr, "A hideous stone statue come to life.");
break;

case 248: // metal gargoyle
strcpy(descpr, "A hideous metal statue come to life.");
break;

case 249: // molten gargoyle
strcpy(descpr, "A hideous melting stone statue come to life.");
break;

case 260: // naga mage
case 261: // naga warrior
strcpy(descpr, "A weird hybrid; human from the waist up, it has a snake tail instead of legs.");
break;

case 262: // orc warlord
strcpy(descpr, "A very large and strong orc.");
break;

case 263: // deep elf soldier
case 264: // deep elf fighter
case 265: // deep elf knight
case 266: // deep elf mage
case 267: // deep elf summoner
case 268: // deep elf conjurer
case 269: // deep elf priest
case 270: // deep elf high priest
case 271: // deep elf demonologist
case 272: // deep elf annihilator
case 273: // deep elf sorceror
case 274: // deep elf death mage
strcpy(descpr, "One of the race of elves which inhabits this dreary cave. ");
break;


/*case 133: // yaktaur
strcpy(descpr, "");
break;*/

case 282:// Ijyb
strcpy(descpr, "A twisted goblin. ");
break;

case 284:// blork
strcpy(descpr, "A particularly fat and ugly orc. ");
break;

case 287:// Erolcha
strcpy(descpr, "A beefy ogre. ");
break;

case 289:// Yrug
strcpy(descpr, "An orc. ");
break;

case 292:// Snorg
strcpy(descpr, "A hairy troll. ");
break;

case 306:// Xtahua
strcpy(descpr, "An ancient and mighty dragon. ");
break;

case 310:// Boris
strcpy(descpr, "An ancient lich, the air around his shrouded form crackling with evil energy. ");
break;




case 220:// white imp
strcpy(descpr, "A small and mischevious minor demon. ");
break;

case 221: // lemure
strcpy(descpr, "A vaguely humanoid blob of putrid white flesh. ");
break;

case 222: // ufetubus
strcpy(descpr, "A chattering and shrieking minor demon. ");
break;

case 223: // manes
strcpy(descpr, "An ugly, twisted little minor demon. ");
break;

case 224: // midge
strcpy(descpr, "A small flying demon. ");
break;

case 225: // neh?
strcpy(descpr, "A weirdly shaped demon. ");
break;

case 226: // orange demon
strcpy(descpr, "A bright orange demon with a venomous stinger. ");
break;

case 227: // hellwing
strcpy(descpr, "A hideous skeletal demon, with wings of ancient withered skin. ");
break;

case 228: // smoke demon
strcpy(descpr, "A writhing cloud of smoke hanging in the air. ");
break;

case 229: // ynoxinul
strcpy(descpr, "A demon with shiny metallic scales. ");
break;

case 230: // Executioner
strcpy(descpr, "A horribly powerful demon. ");
break;

case 231: // green death
strcpy(descpr, "A bloated form covered in oozing sores and exhaling clouds of lethal poison. ");
break;

case 232: // blue death
strcpy(descpr, "A blue greater demon. ");
break;

case 233: // balrug
strcpy(descpr, "A huge and very powerful demon, wreathed in fire and shadows. ");
break;

case 234: // cacodemon
strcpy(descpr, "A hideously ugly demon of rage and legendary power. ");
break;

case 235: // demonic crawler
strcpy(descpr, "A long and bloated body, supported by dozens of short legs and topped with an evil-looking head. ");
break;

case 236: // sun demon
strcpy(descpr, "A demonic figure shining with the light and fury of the sun. ");
break;

case 237: // shadow imp
strcpy(descpr, "A small and shadowy figure. ");
break;

case 238: // shadow demon
strcpy(descpr, "A mysterious demonic figure, constantly blurring into multiple shadows of iteself.");
break;

case 239: // Lorocyproca
strcpy(descpr, "A tall and gaunt figure, draped in long robes which flow as if alive.");
break;



case 251: // Nemelex Xobeh
strcpy(descpr, "A weirdly glowing figure, dancing through the twisted air of Pandemonium. ");
break;

case 252: // Sif Muna
strcpy(descpr, "An ancient and strangely serene demon. It regards you coldly from the huge glowing eye in the centre of its forehead. ");
break;

case 253: // Okawaru
strcpy(descpr, "A violent and wrathful demon, Cerebov appears as a giant human covered in shining golden armour and wielding a huge twisted sword. ");
break;

case 254: // Kikubaaqudgha
strcpy(descpr, "A shadowy figure clothed in impenetrable darkness. ");
break;


case 275: // ooze
strcpy(descpr, "A viscous liquid, flowing along the floor in search of organic matter to corrode. ");
break;

case 276: // azure jelly
strcpy(descpr, "A faintly glowing blob of bright blue cytoplasm. ");
break;

case 277: // death ooze
strcpy(descpr, "A putrid mass of decaying flesh. ");
break;

case 278: // acid blob
strcpy(descpr, "A lump of sickly green flesh, dripping with lethal acid. ");
break;

case 279: // royal jelly
strcpy(descpr, "A particularly rich and golden gelatinous thing. ");
break;



case 280:// Terence
case 281:// Jessica
case 283:// Sigmund
case 285:// Edmund
case 286:// Psyche
case 288:// Donald
case 290:// Michael
case 291:// Joseph
case 293:// Erica
case 294:// Josephine
case 295:// Harold
case 296:// Norbert
case 297:// Jozef
case 298:// Agnes
case 299:// Maud
case 300:// Louise
case 301:// Francis
case 302:// Frances
case 303:// Rupert
case 304:// Wayne
case 305:// Duane
case 307:// Norris
case 308:// Adolf
case 309:// Margery
strcpy(descpr, "A human wandering the dungeons, trying to keep you away from whatever is concealed in the depths. ");
break;


case 340: // Geryon
strcpy(descpr, "A huge and slithery arch-demon, guarding the gates of Hell. ");
break;

case 341: // Dispater
strcpy(descpr, "The lord of the Iron City of Dis. ");
break;

case 342:
strcpy(descpr, "One of the arch-demons who dwell in the depths of Hell. ");
break;

case 343:
strcpy(descpr, "A great titan who lives in the depths of Cocytus. ");
break;

case 344:
strcpy(descpr, "A fearsome arch-fiend who rules the deathly netherworld of Tartarus. ");
break;

case 356:
strcpy(descpr, "A lich who has grown mighty over countless years.");
break;

case 360:
strcpy(descpr, "A heavily armed and armoured guardian of the Vaults. ");
break;

case 361:
strcpy(descpr, "A charred skull floating in the air and rotating slowly. \
Mystic symbols carved into its blackened surface indicate its resistance \
to almost any form of attack. ");
break;

case 362: // vamp knight
strcpy(descpr, "A deadly warrior whose skills have not diminished even in undeath. ");
break;

case 363: // vamp mage
strcpy(descpr, "A powerful magician whose skills have not diminished even in undeath. ");
break;

case 364: // shining eye
strcpy(descpr, "A huge and strangely deformed eyeball, pulsating with light. ");
break;

case 365: // orb guardian
strcpy(descpr, "A huge and glowing purple creature, created by the Orb to defend itself. ");
break;

case 366: // Daeva
strcpy(descpr, "A divine agent of the Shining One. It manifests as a winged figure obscured by an aura of brilliant golden light. ");
break;

case 367: // spectral thing
strcpy(descpr, "A hideous glowing apparition.");
break;

case 368: // greater naga
strcpy(descpr, "An unusually large and powerful naga.");
break;

case 369: // skeletal dragon
strcpy(descpr, "A huge undead abomination, pieced together from the broken bones of many dragons.");
break;

case 370: // tentacled monstrosity
strcpy(descpr, "A writhing mass of tentacles, all covered in putrid mucous.");
break;

case 371: // sphinx
strcpy(descpr, "A large creature with a human head, the body of a lion, and the wings of a huge bird.");
break;

case 372: // rotting hulk
strcpy(descpr, "A shambling undead, related to the ghoul.");
break;

case 373: // guardian mummy
strcpy(descpr, "An ancient warrior, embalmed and cursed with the ability to walk even when dead.");
break;

case 374: // greater mummy
strcpy(descpr, "The embalmed and undead corpse of an ancient ruler.");
break;

case 375: // mummy priest
strcpy(descpr, "The embalmed and undead corpse of an ancient servant of darkness.");
break;

case 376: // centaur warrior
strcpy(descpr, "A hybrid with the torso of a human atop the body of a large horse. It looks aggressive.");
break;

case 377: // yaktaur
strcpy(descpr, "Like a centaur, but half yak. It looks aggressive.");
break;

case 378: // Killer Klown
strcpy(descpr, "A comical figure full of life and laughter. It looks very \
happy to see you.$But is there a slightly malicious cast to its features? \
Is that red facepaint something altogether less pleasant? Join in the fun, \
and maybe you'll find out!");
break;

/* Hugh Cook monsters:*/

case 379: // guardian robot
strcpy(descpr, "A moving column of metal.");
break;

case 380: // dorgi
strcpy(descpr, "\"The dorgi...was a huge grumping machine with a pronounced propensity for violence. It was huge, heavy, brown and bulbous. A hulking \
thing stubbled with inscrutable protruberances. A monstrous thing which moved \
upon its victims with a sound like heavy breathing.\"$(Hugh Cook, _The Wazir \
and the Witch_)");
break;

case 381: // Sword
strcpy(descpr, "An incandescent globe of light, capable of impressive \
pyrotechnics.");
break;

case 382: // quokka
strcpy(descpr, "A small marsupial mammal, altered by nanotechnological manipulators.");
break;


case 385: // eye of devastation
strcpy(descpr, "A huge eyeball, encased in a levitating globe of incandescant energy. ");
break;


case 386: // moth of wrath
strcpy(descpr, "A huge moth, as violent as it is hairy.");
break;


case 387: // death cob
strcpy(descpr, "A dreadful undead cob of corn.");
break;

case 388: // curse toe
strcpy(descpr, "A disembodied toe, hanging in the air and radiating an intense field of negative energy.");
break;

case 389: // mimic in gold form
strcpy(descpr, "An apparently harmless pile of gold coins hides a nasty venomous shapechanging predator.");
break;

case 390: // mimic in weapon form
strcpy(descpr, "An apparently abandoned weapon, actually a vicious little beast in disguise.");
break;

case 391: // mimic in armour form
strcpy(descpr, "An apparently abandoned suit of finely-made armour, actually a vicious little beast in disguise.");
break;

case 392: // mimic in scroll form
strcpy(descpr, "An ancient parchment covered in arcane runes. Did it just twitch?");
break;

case 393: // mimic in potion form
strcpy(descpr, "A delicious looking magical drink. Go on, pick it up!");
break;

case 394: // hell-hog
strcpy(descpr, "A large, fat and very ugly pig, suckled in the pits of Hell. ");
break;

case 395: // Serpent of Hell
strcpy(descpr, "A huge dragon, burning with hellfire. ");
break;

case 396: // boggart
strcpy(descpr, "A twisted little sprite-goblin. Beware of its magical tricks!");
break;

case 397: // quicksilver D
strcpy(descpr, "A long and sinuous dragon, seemingly more neck and tail than anything else. Its skin shines like molten mercury, and magical energies arc from its pointed snout.");
break;

case 398: // iron D
strcpy(descpr, "A very heavy and apparently flightless dragon.");
break;

case 399: // skeletal warrior
strcpy(descpr, "The vicious and heavily armed skeleton of a humanoid creature, animated by unholy power.");
break;

case 400:
strcpy(descpr, "The ghost of ");
strcat(descpr, ghost.gname);
strcat(descpr, ", a");
if (ghost.ghs [12] < 4) strcat(descpr, " weakling ");
else if (ghost.ghs [12] < 7) strcat(descpr, "n average ");
else if (ghost.ghs [12] < 11) strcat(descpr, "n experienced ");
else if (ghost.ghs [12] < 16) strcat(descpr, " powerful ");
else if (ghost.ghs [12] < 22) strcat(descpr, " mighty ");
else if (ghost.ghs [12] < 26) strcat(descpr, " great ");
else if (ghost.ghs [12] < 27) strcat(descpr, "n awesomely powerful ");
else strcat(descpr, " legendary ");
strcat(descpr, species_name(ghost.ghs [9]));
strcat(descpr, " ");
strcat(descpr, skill_title(ghost.ghs [10], ghost.ghs [11], you[0].clas, ghost.ghs [12]));
strcat(descpr, ".");
break;

case 401: // random '&' in pandemonium
describe_demon(descpr);
break;

case MLAVA0: // lava worm
strcpy(descpr, "A vicious red worm which swims through molten rock.");
break;

case MLAVA1: // lava fish
strcpy(descpr, "A fish which lives in lava.");
break;

case MLAVA2: // lava snake
strcpy(descpr, "A burning red snake which rears up from pools of lava and tries to bite you.");
break;

case MWATER0: // big fish
strcpy(descpr, "A fish of unusual size.");
break;

case MWATER1: // giant goldfish
strcpy(descpr, "This is what happens when you give your pet goldfish too much food!");
break;

case MWATER2: // elec eel
strcpy(descpr, "A small and slimy eel, crackling with electrical discharge.");
break;

case MWATER3: // jellyfish
strcpy(descpr, "A pulsating glob of transparent flesh, waiting just below the surface to sting you with its many tentacles.");
break;

case MWATER4: // water elemental
strcpy(descpr, "A spirit drawn from the elemental plane of water. It exists on this world as part of a body of water.");
break;

case MWATER5: // swamp worm
strcpy(descpr, "A large worm, adept at swimming through the muck of this foul swamp.");
break;


default:
strcpy(descpr, "A monster whose description is yet to be written.");
break;


// onocentaur - donkey

/*case MLAVA0: //
strcpy(descpr, "");
break;
*/
/*
case 110: //
strcpy(descpr, "");
break;
*/
}

/*
strcat(descpr, " It looks to be worth about ");
char exper_val [20];
itoa(exper_value(menv [which_mons].m_class, menv [which_mons].m_HD, menv [which_mons].m_hp_max), exper_val, 10);
strcat(descpr, exper_val);
if (exper_value(menv [which_mons].m_class, menv [which_mons].m_HD, menv [which_mons].m_hp_max) == 1)
 strcat(descpr, " experience point.");
  else strcat(descpr, " experience points.");
*/
print_description(descpr);


if (getch() == 0) getch();

#ifdef DOS_TERM
puttext(25, 1, 80, 25, buffer);
window(1, 1, 80, 25);
#endif


} // end void describe_monsters






/*
Describes all items in the game.
*/
void describe_item(int item_class, int item_type, int item_plus, int item_plus2, int item_dam, unsigned char item_id) //, int property [4] [50] [3], int mass [20] [50])
{


   char descpr [1000];

#ifdef DOS_TERM
   char buffer[3400];
   gettext(25, 1, 80, 25, buffer);

   window(25, 1, 80, 25);
#endif
//   strcpy(st_pass, "");

   clrscr();

   strcpy(descpr, "");

int spec_ench = 0;

switch(item_class)
{
case 0: // weapons

if (item_dam % 30 == 25 && strlen(unrandart_descrip(1, item_class, item_type, item_plus, item_plus2)) != 0)
{
  strcpy(descpr, unrandart_descrip(1, item_class, item_type, item_plus, item_plus2));
} else
 switch(item_type)
{
case 0:
strcpy(descpr, "A heavy piece of wood. ");
break;
case 1:
strcpy(descpr, "A long handle with a heavy lump on one end. ");
break;
case 2:
strcpy(descpr, "Like a mace, but with a length of chain between the handle and the lump of metal. ");
break;
case 3:
strcpy(descpr, "A long knife or a very short sword, which can be held or thrown. ");
break;
case 4:
strcpy(descpr, "A mace covered in spikes. ");
break;
case 5:
strcpy(descpr, "A sword with a short, slashing blade. ");
break;
case 6:
strcpy(descpr, "A sword with a long, slashing blade. ");
break;
case 7:
strcpy(descpr, "A sword with a very long, heavy blade and a long handle, designed to be wielded with two hands. ");
break;
case 8:
strcpy(descpr, "A long sword with a curved blade. ");
break;
case 9:
strcpy(descpr, "An axe designed for either hand combat or throwing. ");
break;
case 10:
strcpy(descpr, "A large axe with a double-headed blade, held with two hands. ");
break;
case 11:
strcpy(descpr, "A long stick with a pointy blade on one end, to be held or thrown. ");
break;
case 12:
strcpy(descpr, "A long pole with a spiked axe head on one end. ");
break;
case 13:
strcpy(descpr, "A piece of cloth and leather for launching stones, which do a small amount of damage on impact. ");
break;
case 14:
strcpy(descpr, "A curved piece of wood and string, for shooting arrows. It does good damage in combat, and a skilled user can use it to great effect. ");
break;
case 15:
strcpy(descpr, "A piece of machinery used for firing bolts, which takes some time to load and fire. It does very good damage in combat. ");
break;
case 16:
strcpy(descpr, "A small crossbow, for firing darts. ");
break;
case 17:
strcpy(descpr, "A pole with a large, heavy blade on one end. ");
break;
case 18:
strcpy(descpr, "A pole, held with both hands. ");
break;
case 19:
strcpy(descpr, "A farm implement, usually unsuited to combat. ");
break;
case 20:
strcpy(descpr, "A giant lump of wood, shaped for an ogre's hands. ");
break;
case 21:
strcpy(descpr, "A giant lump of wood covered in sharp spikes. ");
break;
case 22:
strcpy(descpr, "The opposite to a morningstar. ");
break;
case 23:
strcpy(descpr, "A small and magically quick sword. ");
break;
case 24:
strcpy(descpr, "A very rare and extremely effective imported weapon. ");
break;
case 25:
strcpy(descpr, "A huge axe. ");
break;
case 26:
strcpy(descpr, "A magical weapon with two razor-sharp blades. ");
break;
case 27:
strcpy(descpr, "A magical weapon with three great razor-sharp blades. ");
break;
case 28:
strcpy(descpr, "The kind of thing you hit nails with, adapted for battle. ");
break;
case 29:
strcpy(descpr, "A large and vicious toothed club. ");
break;
case 30:
strcpy(descpr, "A whip. ");
break;
case 31:
strcpy(descpr, "A sword with a medium length slashing blade. ");
break;
case 32:
strcpy(descpr, "A terrible weapon, forged in the fires of Hell. ");
break;
case 33:
strcpy(descpr, "A terrible weapon, woven in the depths of the inferno. ");
break;
case 34:
strcpy(descpr, " ");
break;

}
if (item_type < 13 || item_type > 16) // ie is not a missile device
{
strcat(descpr, "$Damage rating: ");
append_value(descpr, property(0, item_type, 0), 0);

strcat(descpr, "$Accuracy rating: ");
append_value(descpr, property(0, item_type, 1), 1);

strcat(descpr, "$Speed multiplier (x10): ");
append_value(descpr, property(0, item_type, 2), 1);
strcat(descpr, "$");

/*if (property(0, item_type, 0) == 3)
strcat(descpr, "It does very little damage in combat, ");
if (property(0, item_type, 0) == 4)
strcat(descpr, "It does a rather small amount of damage in combat, ");
if (property(0, item_type, 0) == 5)
strcat(descpr, "It does a small amount of damage in combat, ");
if (property(0, item_type, 0) == 6)
strcat(descpr, "It does moderate damage in combat, ");
if (property(0, item_type, 0) == 7)
strcat(descpr, "It does a decent amount of damage in combat, ");
if (property(0, item_type, 0) == 8)
strcat(descpr, "It does good damage in combat, ");
// is no 9
if (property(0, item_type, 0) == 10)
strcat(descpr, "It does quite good damage in combat, ");
if (property(0, item_type, 0) == 11)
strcat(descpr, "It does very good damage in combat, ");
if (property(0, item_type, 0) == 12)
strcat(descpr, "It does very, very good damage in combat, ");
if (property(0, item_type, 0) == 13)
strcat(descpr, "It does extremely good damage in combat, ");
if (property(0, item_type, 0) == 15)
strcat(descpr, "It does a great amount of damage in combat, ");
if (property(0, item_type, 0) == 16)
strcat(descpr, "It does a very great amount of damage in combat, ");
if (property(0, item_type, 0) == 17)
strcat(descpr, "It does extremely great damage in combat, ");
if (property(0, item_type, 0) == 19)
strcat(descpr, "It does terrible damage in combat, ");
if (property(0, item_type, 0) == 20)
strcat(descpr, "It does a staggering amount of damage in combat, ");
if (property(0, item_type, 0) == 25)
strcat(descpr, "It does a truly staggering amount of damage in combat, ");

if (property(0, item_type, 1) == -4)
strcat(descpr, "is extremely inaccurate, ");
if (property(0, item_type, 1) == -3)
strcat(descpr, "is very inaccurate, ");
if (property(0, item_type, 1) == -2)
strcat(descpr, "is quite inaccurate, ");
if (property(0, item_type, 1) == -1)
strcat(descpr, "is a bit inaccurate, ");
if (property(0, item_type, 1) == 0)
strcat(descpr, "is not very accurate, ");
if (property(0, item_type, 1) == 1)
strcat(descpr, "is a bit accurate, ");
if (property(0, item_type, 1) == 2)
strcat(descpr, "is somewhat accurate, ");
if (property(0, item_type, 1) == 3)
strcat(descpr, "is quite accurate, ");
if (property(0, item_type, 1) == 4)
strcat(descpr, "is very accurate, ");
if (property(0, item_type, 1) == 5)
strcat(descpr, "is greatly accurate, ");
if (property(0, item_type, 1) == 6)
strcat(descpr, "is extremely accurate, ");

if (property(0, item_type, 2) == 7) / * quick blade * /
strcat(descpr, "and is lightning fast. ");
if (property(0, item_type, 2) == 11)
strcat(descpr, "and is extremely quick. ");
if (property(0, item_type, 2) == 12)
strcat(descpr, "and is quite quick. ");
if (property(0, item_type, 2) == 13)
strcat(descpr, "and is quick. ");
if (property(0, item_type, 2) == 14)
strcat(descpr, "and is a little slow. ");
if (property(0, item_type, 2) == 15)
strcat(descpr, "and is quite slow. ");
// no 16
if (property(0, item_type, 2) == 17)
strcat(descpr, "and is very slow. ");
if (property(0, item_type, 2) == 18)
strcat(descpr, "and is extremely slow. ");
if (property(0, item_type, 2) == 19)
strcat(descpr, "and is very cumbersome. ");
if (property(0, item_type, 2) == 22)
strcat(descpr, "and is extremely cumbersome. ");*/
}// else
//{
//}
if (item_plus >= 100 && item_id >= 1)
{
 strcat(descpr, "It has a curse placed upon it. ");
 item_plus -= 100;
}
if (item_id >= 3 && !(item_class == 0 && item_dam % 30 >= 25))
{
if (item_plus < 50)
{
 strcat(descpr, "It has been damaged to be less accurate. ");
}
if (item_plus > 50)
{
 strcat(descpr, "It has been ");
 print_ench(descpr, item_plus);
 strcat(descpr, "to be more accurate. ");
}
if (item_plus2 < 50)
{
 strcat(descpr, "It has been damaged to cause less damage. ");
}
if (item_plus2 > 50)
{
 strcat(descpr, "It has been ");
 print_ench(descpr, item_plus2);
 strcat(descpr, "to inflict greater damage. ");
}
}
if (item_dam >= 180)
{
if (item_id < 2) break;
switch(item_dam)
{
  case 181: strcat(descpr, "This blessed weapon loves nothing more than to sing to its owner, whether they want it to or not. "); break;
  case 182: strcat(descpr, "This was the favourite weapon of the Old God Trog, before he lost it one day. It induces a bloodthirsty berserker rage in anyone who uses it to strike another. "); break;
  case 183: strcat(descpr, "This weapon carries a terrible and highly irritating curse. "); break;
  case 184: strcat(descpr, "It is rather unreliable. "); break;
  case 185: strcat(descpr, "It is the creation of a mad God, and carries a curse which transforms anyone possessing it into a prune. Fortunately, the curse works very slowly, and one can use it briefly with no consequences worse than slightly purple skin and a few wrinkles. "); break;
  case 186: strcat(descpr, "This truly accursed weapon is an instrument of Hell. "); break;
  case 187: strcat(descpr, "This dreadful weapon is used at the user's peril. "); break;
  case 188: strcat(descpr, "Flames cover its twisted blade. "); break;
  case 189: strcat(descpr, "This legendary item can unleash the fury of Hell. "); break;
  case 190: strcat(descpr, "It carries some of the powers of the arch-fiend Asmodeus. "); break;
  case 191: strcat(descpr, "It rewards the powerful with power and the meek with weakness. "); break;
  case 192: strcat(descpr, "It is almost unerringly accurate. "); break;
  case 193: strcat(descpr, "It was the magical weapon wielded by the mighty wizard Olgreb before he met his fate somewhere within these dungeons. It grants its wielder resistance to the effects of poison and increases their ability to use venomous magic, and carries magical powers which can be invoked by one skilled in the use of magic. "); break;
  case 194: strcat(descpr, "It is lethally vampiric. "); break;
  case 195: strcat(descpr, "Its power varies in proportion to its wielder's intelligence. Using it can be a bit risky. "); break;
}
break;
}

spec_ench = item_dam % 30;
if (item_dam % 30 >= 25) spec_ench = randart_wpn_properties(item_class, item_type, item_dam, item_plus, item_plus2, 0, RAP_BRAND);

if (spec_ench != 0 && item_id >= 2)
{
switch(spec_ench)
{
  case 1: strcat(descpr, "It emits flame when wielded, causing extra injury to most foes and up to double damage against particularly susceptible opponents. "); break;
  case 2: strcat(descpr, "It has been specially enchanted to freeze those struck by it, causing extra injury to most foes and up to double damage against particularly susceptible opponents. "); break;
  case 3: strcat(descpr, "It has been blessed by the Shining One to harm undead and cause great damage to the unholy creatures of Hell or Pandemonium. "); break;
  case 4: strcat(descpr, "Occasionally upon striking a foe it will discharge some electrical energy and cause terrible harm. "); break;
  case 5: strcat(descpr, "It is especially effective against all of orcish descent. "); break;
  case 6: strcat(descpr, "It poisons the flesh of those it strikes. "); break;
  case 7: strcat(descpr, "It protects the one who wields it against injury. "); break;
  case 8: strcat(descpr, "A truly terrible weapon, it drains the life of those it strikes. "); break;
  case 9: strcat(descpr, "It allows its wielder to attack twice when they would otherwise have struck only once. "); break;
  case 10: strcat(descpr, "It inflicts extra damage upon your enemies. "); break;
  case 11: strcat(descpr, "It turns projectiles fired from it into bolts of fire. "); break;
  case 12: strcat(descpr, "It turns projectiles fired from it into bolts of frost. "); break;
  case 13: strcat(descpr, "It inflicts no extra harm, but heals its wielder somewhat when he or she strikes a living foe. "); break;
  case 14: strcat(descpr, "It is a weapon blessed by Zin, and can inflict up to fourfold damage when used against the undead. "); break;
  case 15: strcat(descpr, "In the hands of one skilled in necromantic magic it inflicts extra damage on living creatures. "); break;
}
}
/* randart properties: */
if (item_id >= 2 && item_dam % 30 >= 25)
{
 randart_descpr(descpr, item_class, item_type, item_plus, item_plus2, item_dam);
}

switch(weapon_skill(item_class, item_type))
{
  case 1: strcat(descpr, "It falls into the 'short blades' category. "); break;
  case 2: strcat(descpr, "It falls into the 'long swords' category. "); break;
  case 3: strcat(descpr, "It falls into the 'great swords' category. "); break;
  case 4: strcat(descpr, "It falls into the 'axes' category. "); break;
  case 5: strcat(descpr, "It falls into the 'maces and flails' category. "); break;
  case 6: strcat(descpr, "It falls into the 'pole-arms' category. "); break;
  case 7: strcat(descpr, "It falls into the 'staves' category. "); break;
  case 8: strcat(descpr, "It falls into the 'slings' category. "); break;
  case 9: strcat(descpr, "It falls into the 'bows' category. "); break;
  case 10: strcat(descpr, "It falls into the 'crossbows' category. "); break;

}
break; // end weapons

case 1: // ammunition
switch(item_type)
{
case 0:
strcpy(descpr, "A stone. ");
break;
case 1:
strcpy(descpr, "An arrow. ");
break;
case 2:
strcpy(descpr, "A crossbow bolt. ");
break;
case 3:
strcpy(descpr, "A small throwing weapon. ");
break;
case 4:
strcpy(descpr, "A purple vegetable. The presence of this object in the game indicates a bug (or some kind of cheating on your part). ");
break;
case 5:
strcpy(descpr, "A rock, used by giants as a missile. ");
break;
}
if (item_dam % 30 != 0 && item_id >= 2)
{
 switch(item_dam % 30)
 {
  case 1: strcat(descpr, "When fired from an appropriate launcher, it turns into a bolt of flame. "); break;
  case 2: strcat(descpr, "When fired from an appropriate launcher, it turns into a bolt of ice. "); break;
  case 3:
  case 4: strcat(descpr, "It is coated with poison. "); break;
 }
}
if (item_id == 3 && item_plus > 50)
{
 strcat(descpr, "It has been ");
 print_ench(descpr, item_plus);
 strcat(descpr, "to be more accurate and cause more damage. ");
}
break;

case 2: // armour
if (item_dam % 30 == 25 && strlen(unrandart_descrip(1, item_class, item_type, item_plus, item_plus2)) != 0)
{
  strcpy(descpr, unrandart_descrip(1, item_class, item_type, item_plus, item_plus2));
} else
 switch(item_type)
{
case 0:
strcpy(descpr, "A cloth robe. ");
break;
case 1:
strcpy(descpr, "A suit made of hardened leather. ");
break;
case 2:
strcpy(descpr, "A leather suit covered in little rings. ");
break;
case 3:
strcpy(descpr, "A leather suit covered in little metal plates. ");
break;
case 4:
strcpy(descpr, "A suit made of interlocking metal rings. ");
break;
case 5:
strcpy(descpr, "A suit made of splints of metal. ");
break;
case 6:
strcpy(descpr, "A suit made of bands of metal. ");
break;
case 7:
strcpy(descpr, "A suit of mail and large plates of metal. ");
break;
case 8:
strcpy(descpr, "A piece of metal, to be strapped on one's arm. ");
break;
case 9:
strcpy(descpr, "A cloth cloak. ");
break;
case 10:
if (item_plus2 == 0) strcpy(descpr, "A piece of metal headgear. ");
if (item_plus2 == 1) strcpy(descpr, "A piece of metal headgear. ");
if (item_plus2 == 2) strcpy(descpr, "A cloth or leather cap. ");
if (item_plus2 == 3) strcpy(descpr, "A conical cloth hat. ");
break;
case 11:
strcpy(descpr, "A pair of gloves. ");
break;
case 12:
if (item_plus2 == 1) strcpy(descpr, "A special armour made for Nagas, to wear on their tails. "); else
if (item_plus2 == 2) strcpy(descpr, "An armour made for centaurs, to wear on their horse bodies. "); else
strcpy(descpr, "A pair of sturdy boots. ");
break;
case 13:
strcpy(descpr, "A small shield. ");
break;
case 14:
strcpy(descpr, "Like a normal shield, only larger. ");
break;
case 15:
strcpy(descpr, "The scaly skin of a dragon. I suppose you could wear it if you really wanted to. ");
break;
case 16:
strcpy(descpr, "The rough and knobbly hide of a troll. I suppose you could wear it if you really wanted to. ");
break;
case 17:
strcpy(descpr, "An incredibly heavy but extremely effective suit of crystalline armour. ");
break;
case 18:
strcpy(descpr, "A magical armour, made from the scales of a fire-breathing dragon. It provides great protection from the effects of fire, but renders its wearer more susceptible to the effects of cold. ");
break;
case 19:
strcpy(descpr, "A magical armour, made from the skin of a common troll. It magically regenerates its wearer's flesh at a fairly slow rate (unless they're already a troll). ");
break;
case 20:
strcpy(descpr, "The scaly skin of a dragon. I suppose you could wear it if you really wanted to. ");
break;
case 21:
strcpy(descpr, "A magical armour, made from the scales of a cold-breathing dragon. It provides great protection from the effects of cold, but renders its wearer more susceptible to the effects of fire and heat. ");
break;
case 22:
strcpy(descpr, "The soft and supple scaley skin of a steam dragon. I suppose you could wear it if you really wanted to. ");
break;
case 23:
strcpy(descpr, "A magical armour, made from the scales of a steam-breathing dragon. Although unlike the armour made from the scales of some larger dragons it does not provide its wearer with any special magical protection, it is extremely light and as supple as cloth. ");
break;
case 24:
strcpy(descpr, "The weirdly-patterned scaley skin of a mottled dragon. I suppose you could wear it if you really wanted to. ");
break;
case 25:
strcpy(descpr, "A magical armour made from the scales of a mottled dragon. Although unlike the armour made from the scales of some larger dragons it does not provide its wearer with any special magical protection, it is as light and relatively uncumbersome as leather armour. ");
break;
case 26:
strcpy(descpr, "The hide of a storm dragon, covered in extremely hard blue scales. I suppose you could wear it if you really wanted to. ");
break;
case 27:
strcpy(descpr, "A magical armour made from the scales of a lightning-breathing dragon. It is heavier than most dragon scale armours, but gives its wearer great resistance to electrical discharges. ");
break;
case 28:
strcpy(descpr, "The extremely tough and heavy skin of a golden dragon, covered in shimmering golden scales. I suppose you could wear it if you really wanted to. ");
break;
case 29:
strcpy(descpr, "A magical armour made from the golden scales of a golden dragon. It is extremely heavy and cumbersome, but confers resistance to fire, cold, and poison on its wearer. ");
break;
case 30:
strcpy(descpr, "The skins of several animals. ");
break;
case 31:
strcpy(descpr, "The slimy and smelly skin of a swamp-dwelling dragon. I suppose you could wear it if you really wanted to. ");
break;
case 32:
strcpy(descpr, "A magical armour made from the scales of a swamp dragon. It confers resistance to poison on its wearer. ");
break;
}
if (item_type != 8 && item_type != 13 && item_type != 14)
{
 strcat(descpr, "$Armour rating: ");
 append_value(descpr, property(2, item_type, 0), 0);
 strcat(descpr, "$Evasion modifier: ");
 append_value(descpr, property(2, item_type, 1), 0);
 strcat(descpr, "$");
}

if (item_plus >= 100 && item_id >= 1)
{
 strcat(descpr, "It has a curse placed upon it. ");
 item_plus -= 100;
}
if (item_id >= 3)
{
if (item_plus < 50)
{
 strcat(descpr, "It has been damaged to offer less protection against injury. ");
}
if (item_plus > 50)
{
 strcat(descpr, "It has been ");
 print_ench(descpr, item_plus);
 strcat(descpr, "to be more effective in reducing injury. ");
}
}

if (item_id >= 2 && item_dam % 30 >= 25)
       randart_descpr(descpr, item_class, item_type, item_plus, item_plus2, item_dam);

if (item_id > 1)
{
 switch(item_dam % 30)
 {
  case 1: strcat(descpr, "It allows its wearer to run at a great speed. "); break;
  case 2: strcat(descpr, "It protects its wearer from heat and fire. "); break;
  case 3: strcat(descpr, "It protects its wearer from cold. "); break;
  case 4: strcat(descpr, "It protects its wearer from poison. "); break;
  case 5: strcat(descpr, "It allows its wearer to see invisible things. "); break;
  case 6: strcat(descpr, "When activated it hides its wearer from the sight of others, but also increases their metabolic rate by a large amount. "); break;
  case 7: strcat(descpr, "It increases the physical power of its wearer. "); break;
  case 8: strcat(descpr, "It increases the dexterity of its wearer. "); break;
  case 9: strcat(descpr, "It makes you more cleverer. "); break;
 case 10: strcat(descpr, "It is highly cumbersome. "); break;
 case 11: strcat(descpr, "It can be activated to allow its wearer to float above the ground. "); break;
 case 12: strcat(descpr, "It increases its wearer's resistance to enchantments. "); break;
 case 13: strcat(descpr, "It protects its wearer from harm. "); break;
 case 14: strcat(descpr, "It enhances the stealth of its wearer. "); break;
 case 15: strcat(descpr, "It protects its wearer from the effects of both cold and heat. "); break; // these two are robes only.
 case 16: strcat(descpr, "It protects its wearer from the effects of negative energy. "); break;
 case 17: strcat(descpr, "It greatly increases the power of its wearer's magical spells, but is only intended for those who have very little left to learn. "); break;
 case 18: strcat(descpr, "It protects its wearer's possessions from damage and destruction. "); break;
 }
}
break;

case 3:
if (get_id(3, item_type) == 0)
{
 strcpy(descpr, "A stick. Maybe it's magical. ");
 break;
}
strcpy(descpr, "A magical device which");
switch (item_type)
{
case 0:
strcat(descpr, " throws little bits of flame. ");
break;
case 1:
strcat(descpr, " throws little bits of frost. ");
break;
case 2:
strcat(descpr, " casts enchantments to slow down the actions of a creature at which it is directed. ");
break;
case 3:
strcat(descpr, " casts enchantments to speed up the actions of a creature at which it is directed. ");
break;
case 4:
strcat(descpr, " throws small bolts of destructive energy. ");
break;
case 5:
strcat(descpr, " can heal a creature's wounds. ");
break;
case 6:
strcat(descpr, " can hold a creature immobile. ");
break;
case 7:
strcat(descpr, " throws great bolts of fire. ");
break;
case 8:
strcat(descpr, " throws great bolts of cold. ");
break;
case 9:
strcat(descpr, " induces confusion and bewilderment in a target creature. ");
break;
case 10:
strcat(descpr, " hides a creature from the view of others. ");
break;
case 11:
strcat(descpr, " drills tunnels through unworked rock. ");
break;
case 12:
strcat(descpr, " throws exploding blasts of flame. ");
break;
case 13:
strcat(descpr, " causes a creature to be randomly translocated. ");
break;
case 14:
strcat(descpr, " throws great bolts of lightning. ");
break;
case 15:
strcat(descpr, " causes a creature to be transmogrified into another form. It doesn't work on you, so don't even try. ");
break;
case 16:
strcat(descpr, " causes slavish obedience in a creature. ");
break;
case 17:
strcat(descpr, " throws a bolt of negative energy which drains the life essences of living creatures, but is useless against the undead. ");
break;
case 18:
strcat(descpr, " can produce a variety of effects. ");
break;
case 19:
strcat(descpr, " disrupts the physical structure of an object, especially a creature's body. ");
break;
}
if (item_id == 3 && item_plus == 0)
{
strcat(descpr, "Unfortunately, it has no charges left. ");
}
break;

case 4: // food
switch (item_type)
{
case 0:
strcat(descpr, "A filling ration of dried and preserved meats. ");
break;
case 1:
strcat(descpr, "A filling ration of bread. ");
break;
case 2:
strcat(descpr, "A delicious juicy fruit. ");
break;
case 3:
strcat(descpr, "A delicious red or green fruit. ");
break;
case 4:
strcat(descpr, "An almost tasteless green vegetable, which grows on a vine. ");
break;
case 5:
strcat(descpr, "A lump of the delicious honeycomb made by giant bees. ");
break;
case 6:
strcat(descpr, "A lump of the magical substance produced by giant bees to be fed to their queens. ");
break;
case 7:
strcat(descpr, "A repulsive cucumber-shaped vegetable. ");
break;
case 8:
strcat(descpr, "A slice of pizza. Don't tell me you don't know what that is! ");
break;
case 9:
strcat(descpr, "A delicious orange fruit. ");
break;
case 10:
strcat(descpr, "A delicious juicy orange fruit. ");
break;
case 11:
strcat(descpr, "A delicious yellow fruit, probably grown and imported by some amoral multinational as the result of a corrupt trade deal. ");
break;
case 12:
strcat(descpr, "A small but delicious red fruit. ");
break;
// are you noticing a pattern here?
case 13:
strcat(descpr, "A small but delicious tropical fruit. How it got into this dungeon is anyone's guess. ");
break;
case 14:
strcat(descpr, "A yellow fruit. ");
break;
case 15:
strcat(descpr, "A small fruit. ");
break;
case 16:
strcat(descpr, "A dried grape. ");
break;
case 17:
strcat(descpr, "A tropical fruit. ");
break;
case 18:
strcat(descpr, "A strip of preserved dead cow. ");
break;
case 19:
strcat(descpr, "A lump of cheese. ");
break;
case 20:
strcat(descpr, "An elongated lump of low-grade gristle, entrails and cereal products encased in an intestine. Yum! ");
break;
case 21:
strcat(descpr, "A piece of dungeon meat. It looks rather unpleasant. ");
if (item_dam < 100) strcat (descpr, "In fact, it is rotting away before your eyes. Eating it would probably be unwise. ");
break;
}
/*
case 0:
strcat(descpr, ". ");
break;
*/
break;

case 8:
if (get_id(8, item_type) == 0)
{
strcat(descpr, "A small bottle of liquid. ");
break;
}
switch(item_type)
{
case 0:
strcat(descpr, "A blessed fluid which heals some wounds, clears the mind, and cures diseases. If one uses it when they are at or near full health, it can also slightly repair permanent injuries. ");
break;
case 1:
strcat(descpr, "A magical healing elixir, which causes wounds to close and heal almost instantly. If one uses it when they are at or near full health, it can also repair permanent injuries. ");
break;
case 2:
strcat(descpr, "An enchanted beverage which speeds the actions of anyone who drinks it. ");
break;
case 3:
strcat(descpr, "A magic potion which greatly increases the strength and physical power of one who drinks it. ");
break;
case 4:
strcat(descpr, "A potion of beneficial mutation. ");
break;
case 5:
strcat(descpr, "A potion of beneficial mutation. ");
break;
case 6:
strcat(descpr, "A potion of beneficial mutation. ");
break;
case 7:
strcat(descpr, "A potion which confers great buoyancy on one who consumes it. ");
break;
case 8:
strcat(descpr, "A nasty poisonous liquid. ");
break;
case 9:
strcat(descpr, "A potion which slows your actions. ");
break;
case 10:
strcat(descpr, "A potion which eliminates your control over your own body. ");
break;
case 11:
strcat(descpr, "A potion which confuses your perceptions and reduces your control over your own actions. ");
break;
case 12:
strcat(descpr, "A potion which hides you from the sight of others. ");
break;
case 13:
strcat(descpr, "A filling potion of sludge, high in cereal fibre. ");
break;
case 14:
strcat(descpr, "A noxious concoction, which can do terrible things to your body, brain and reflexes. ");
break;
case 15:
strcat(descpr, "A vile and putrid cursed liquid, which causes your flesh to decay before your very eyes. ");
break;
case 16:
strcat(descpr, "A unique substance, vital for the existence of most life. ");
break;
case 17:
strcat(descpr, "A truly wonderful and very rare drink. ");
break;
case 18:
strcat(descpr, "A valuable potion which grants a person with an infusion of magical energy. ");
break;
case 19:
strcat(descpr, "A potion which restores the abilities of one who drinks it. ");
break;
case 20:
strcat(descpr, "A terribly venomous potion. ");
break;
case 21:
strcat(descpr, "A potion which can send one into an incoherent rage. ");
break;
case 22:
strcat(descpr, "A potion which removes some or all of any mutations which may be afflicting you. ");
break;
case 23:
strcat(descpr, "A potion which does very strange things to you. ");
break;
}
break; // end potions

case 6:
if (get_id(6, item_type) == 0)
{
 strcat(descpr, "A scroll of paper covered in magical writing. ");
 break;
}
switch(item_type)
{
case 0:
strcat(descpr, "This useful magic scroll allows you to determine the properties of any object. ");
break;
case 1:
strcat(descpr, "Reading the words on this scroll translocates you to a random position. ");
break;
case 2:
strcat(descpr, "This scroll causes great fear in those who see the one who reads it. ");
break;
case 3:
strcat(descpr, "This scroll causes a loud noise. It is not universally noted for its usefulness.");
break;
case 4:
strcat(descpr, "Reading this scroll removes curses from the items you are using. ");
break;
case 5:
strcat(descpr, "This scroll allows you to detect the presence of cursed items among your possessions. ");
break;
case 6:
strcat(descpr, "This scroll opens a conduit to the Abyss and draws a terrible beast to this world for a limited time. ");
break;
case 7:
strcat(descpr, "This scroll places an enchantment on a weapon, making it more accurate in combat. It may fail to affect weapons already heavily enchanted. ");
break;
case 8:
strcat(descpr, "This scroll places an enchantment on a piece of armour. ");
break;
case 9:
strcat(descpr, "This scroll calls on the powers of Hell to inflict great pain on any nearby creature - including you! ");
break;
case 10:
strcat(descpr, "Does some randomly useless thing. ");
break;
case 11:
strcat(descpr, "This scroll places a curse on a weapon. ");
break;
case 12:
strcat(descpr, "This scroll places a curse on a piece of armour. ");
break;
case 13:
strcat(descpr, "Upon being read, this scroll causes a large explosion. ");
break;
case 14:
strcat(descpr, "This scroll allows its reader to teleport a short distance, with precise control. ");
break;
case 15:
strcat(descpr, "Apart from a label, this scroll is blank. ");
break;
case 16:
strcat(descpr, "This scroll reveals the nearby surroundings of one who reads it. ");
break;
case 17:
strcat(descpr, "This scroll induces an irritating disorientation. ");
break;
case 18:
strcat(descpr, "This wonderful scroll causes the creation of an item valuable to the reader at the reader's feet. It is especially treasured by specialist magicians, as they can use it to obtain the powerful spells of their speciality. It will avoid creating some types of item if one such is already in the possession of the reader. ");
break;
case 19:
strcat(descpr, "This scroll places an enchantment on a weapon, making it inflict greater damage in combat. It may fail to affect weapons already heavily enchanted. ");
break;
case 20: // vorpalise weapon
strcat(descpr, "This scroll enchants a weapon so as to make it far more effective at inflicting harm on its wielder's enemies. Using it on a weapon already affected by some kind of special enchantment (other than that produced by a normal scroll of enchant weapon) is not advised. ");
break;
case 21:
strcat(descpr, "This scroll restores the charges of any magical wand wielded by its reader. ");
break;
case 22:
strcat(descpr, "This scroll enchants a weapon to be far more effective in combat. Although it can be used in the creation of especially enchanted weapons, it may fail to affect those already heavily enchanted. ");
break;
}
break;

case 7:
if (item_dam == 201 && strlen(unrandart_descrip(1, item_class, item_type, item_plus, item_plus2)) != 0)
{
  strcpy(descpr, unrandart_descrip(1, item_class, item_type, item_plus, item_plus2));
} else
 if (get_id(7, item_type) == 0)
{
strcat(descpr, "A piece of jewellery. ");
} else
 switch(item_type)
{
case 0:
strcat(descpr, "This wonderful ring greatly increases the recuperative powers of its wearer, but also considerably speeds his or her metabolism. ");
break;
case 1:
strcat(descpr, "This ring either protects its wearer from harm or makes them more vulnerable to injury, to a degree dependent on its power. ");
break;
case 2:
strcat(descpr, "This ring provides protection from heat and fire. ");
break;
case 3:
strcat(descpr, "This ring provides protection from the effects of poisons and venom. ");
break;
case 4:
strcat(descpr, "This ring provides protection from cold. ");
break;
case 5:
strcat(descpr, "This ring increases or decreases the physical strength of its wearer, to a degree dependent on its power. ");
break;
case 6:
strcat(descpr, "This ring increases the hand-to-hand and missile combat skills of its wearer. ");
break;
case 7:
strcat(descpr, "This ring allows its wearer to see those things hidden from view by magic. ");
break;
case 8:
strcat(descpr, "This powerful ring can be activated to hide its wearer from the view of others, but increases the speed of his or her metabolism greatly while doing so. ");
break;
case 9:
strcat(descpr, "This accursed ring causes its wearer to hunger considerably more quickly. ");
break;
case 10:
strcat(descpr, "This ring occasionally exerts its power to randomly translocate its wearer to another place, and can be deliberately activated for the same effect. ");
break;
case 11:
strcat(descpr, "This ring makes its wearer either more or less capable of avoiding attacks, depending on its degree of enchantment. ");
break;
case 12:
strcat(descpr, "This ring protects its wearer from the loss of their strength, dexterity and intelligence. ");
break;
case 13:
strcat(descpr, "This ring provides energy to its wearer, so that they need eat less often. ");
break;
case 14:
strcat(descpr, "This ring increases or decreases the dexterity of its wearer, depending on the degree to which it has been enchanted. ");
break;
case 15:
strcat(descpr, "This ring increases or decreases the mental ability of its wearer, depending on the degree to which it has been enchanted. ");
break;
case 16:
strcat(descpr, "This ring increases the ability of its wearer to use magical spells. ");
break;
case 17:
strcat(descpr, "This ring increases its wearer's reserves of magical power. ");
break;
case 18:
strcat(descpr, "This ring causes its wearer to hover above the floor. ");
break;
case 19:
strcat(descpr, "This blessed ring protects the life-force of its wearer from negative energy, making them immune to the draining effects of undead and necromantic magic. ");
break;
case 20:
strcat(descpr, "This ring increases its wearer's resistance to hostile enchantments. ");
break;
case 21:
strcat(descpr, "This ring brings its wearer more in contact with the powers of fire. He or she gains resistance to heat and can use fire magic more effectively, but becomes more vulnerable to the effects of cold. ");
break;
case 22:
strcat(descpr, "This ring brings its wearer more in contact with the powers of cold and ice. He or she gains resistance to cold and can use ice magic more effectively, but becomes more vulnerable to the effects of fire. ");
break;
case 23:
strcat(descpr, "This ring allows its wearer to control the destination of any teleportation, although without perfect accuracy. Trying to teleport into a solid object will result in a random teleportation, at least in the case of a normal teleportation. ");
break;
case 35:
strcat(descpr, "This amulet enables its wearer to attempt to enter a state of berserk rage, and increases their chance of successfully doing so. ");
break;
case 36:
strcat(descpr, "This amulet protects its wearer from some magically induced forms of slowness, and increases the duration of enchantments which speed his or her actions. ");
break;
case 37:
strcat(descpr, "This amulet protects its wearer from some forms of mental confusion. ");
break;
case 38:
strcat(descpr, "This amulet repels some of the attacks of creatures which have been magically summoned. ");
break;
case 39:
strcat(descpr, "This amulet protects the armour and weaponry of its wearer from corrosion caused by acids, although not infallibly so. ");
break;
case 40:
strcat(descpr, "This amulet allows its wearer to consume meat in various states of decay without suffering unduly as a result. Poisonous or cursed flesh is still not recommended. ");
break;
case 41:
strcat(descpr, "This amulet protects some of the possessions of its wearer from outright destruction, but not infallibly so. ");
break;
case 42:
strcat(descpr, "Should the wearer of this amulet be levitated by magical means, he or she will be able to exercise some control over the resulting motion. This allows the descent of staircases and the retrieval of items lying on the ground, for example, but does not deprive the wearer of the benefits of levitation. ");
break;
case 43:
strcat(descpr, "This amulet makes its wearer less accurate in hand combat. ");
break;
case 44:
strcat(descpr, "This amulet protects its wearer from mutations, although not infallibly so. ");
break;

}
if (item_id > 0 && item_plus >= 100)
{
strcat(descpr, "It has a curse placed upon it. ");
}
/* randart properties: */
if (item_id >= 2 && (item_dam == 200 || item_dam == 201))
{
 randart_descpr(descpr, item_class, item_type, item_plus, item_plus2, item_dam);
}
break;

case 10: // books
switch (item_type)
{
case 35:
strcat(descpr, "An extremely powerful but unpredictable book of magic. ");
break;
case 41:
strcat(descpr, "A valuable book of magic which allows one to practise a certain skill greatly. As it is used, it gradually disintegrates and will eventually fall apart. ");
break;
default: strcat(descpr, "A book of magic spells. Beware, for some of the more powerful grimoires are not to be toyed with. "); break;
}
break;


case 11: // more magical devices
if (item_id == 0)
{
strcat(descpr, "A stick embued with magical properties. ");
break;
} // end if
switch(item_type)
{
case 0:
strcat(descpr, "This staff increases the magical proficiency of its wielder by a considerable degree, increasing the power of their spells. ");
break;
case 1:
strcat(descpr, "This staff provides a reservoir of magical power to its wielder. ");
break;
case 2:
strcat(descpr, "This staff increases the power of fire spells cast by its wielder, and protects him or her from the effects of heat and fire. ");
break;
case 3:
strcat(descpr, "This staff increases the power of ice spells cast by its wielder, and protects him or her from the effects of cold. ");
break;
case 4:
strcat(descpr, "This staff increases the power of poisoning spells cast by its wielder, and protects him or her from the effects of poison. ");
break;
case 5:
strcat(descpr, "This staff allows its wielder to cast magical spells without hungering as a result. ");
break;
case 6:
strcat(descpr, "This staff increases the power of necromantic spells cast by its wielder. ");
break;
case 7:
strcat(descpr, "This staff increases the power of conjurations cast by its wielder. ");
break;
case 8:
strcat(descpr, "This staff increases the power of enchantments cast by its wielder. ");
break;
case 9:
strcat(descpr, "This staff increases the power of summonings cast by its wielder. ");
break;
case 10:
strcat(descpr, "This holy spell staff allows its wielder to smite foes from afar. ");
break;
case 11:
strcat(descpr, "This spell staff contains spells of summoning. ");
break;
case 16:
strcat(descpr, "This spell staff contains spells designed to repel one's enemies. ");
break;
case 17:
strcat(descpr, "This spell staff contains spells which reveal various aspects of an explorer's surroundings to them. ");
break;

case 25:
strcat(descpr, "This staff increases the power of air spells cast by its wielder. ");
break;
case 26:
strcat(descpr, "This staff increases the power of earth spells cast by its wielder. ");
break;
case 27:
strcat(descpr, "This staff allows its caster to channel ambient magical energy for his or her own purposes. ");
break;


default:
strcat(descpr, "This spell staff contains spells of mayhem and destruction. ");
break;
}
if (item_type >= 10 && item_type < 25)
{
 strcat(descpr, "Casting a spell from it consumes no food, and will not fail. ");
}
strcat(descpr, "$Damage rating: 7 $Accuracy rating: +6 $Speed multiplier (x10): +12");

strcat(descpr, "$It falls into the 'staves' category. ");
break;

case 12:
strcat(descpr, "Once you have escaped to the surface with this invaluable artefact, your quest is complete.");
break;

case 13: // Miscellaneous
switch(item_id)
{
case 0:
 switch(item_type)
 {
 case 0: strcat(descpr, "A heavy bronze flask, warm to the touch. "); break;
 case 10:
 case 12:
 case 1: strcat(descpr, "A sphere of clear crystal. "); break;
 case 2: strcat(descpr, "A fan. "); break;
 case 3: strcat(descpr, "A lamp. "); break;
 case 4: strcat(descpr, "A lump of rock. "); break;
 case 5: strcat(descpr, "A strange lantern made out of ancient bones. "); break;
 case 6: strcat(descpr, "A great silver horn, radiating unholy energies. "); break;
 case 7: strcat(descpr, "A small black box. I wonder what's inside? "); break;
 case 8:
 case 15:
 case 16:
 case 9: strcat(descpr, "A deck of cards. "); break;
 case 11: strcat(descpr, "A small black box. I wonder what's inside? "); break;
 case 13: strcat(descpr, "A grey disc. "); break;
 case 17: strcat(descpr, "An altar to Nemelex Xobeh, built for easy assembly and disassembly. Invoke it to place on a clear patch of floor, then pick it up again when you've finished. "); break;
 }
break;

default:
 switch(item_type)
 {
 case 0: strcat(descpr, "A mighty efreet, captured by some wizard and bound into a bronze flask. Breaking the flask's seal will release it to wreak havoc - possibly on you. "); break;
 case 1: strcat(descpr, "A magical device which allows one to see the layout of their surroundings. It requires a degree of magical ability to be used reliably, otherwise it can produce unpredictable and possibly harmful results. "); break;
 case 2: strcat(descpr, "A magical device for summoning air elementals. It is rather unreliable, and usually requires several attempts to function correctly. Using it carries an element of risk, which is reduced if one is skilled in the appropriate elemental magic. "); break;
 case 3: strcat(descpr, "A magical device for summoning fire elementals. It is rather unreliable, and usually requires several attempts to function correctly. Using it carries an element of risk, which is reduced if one is skilled in the appropriate elemental magic. "); break;
 case 4: strcat(descpr, "A magical device for summoning earth elementals. It is rather unreliable, and usually requires several attempts to function correctly. Using it carries an element of risk, which is reduced if one is skilled in the appropriate elemental magic. "); break;
 case 5: strcat(descpr, "An unholy device which calls on the powers of darkness to assist its user, with a small cost attached. "); break;
 case 6: strcat(descpr, "The horn belonging to Geryon, guardian of the Vestibule of Hell. Legends say that a mortal who desires access into one of the Hells must use it in order to gain entry. "); break;
 case 7: strcat(descpr, "A magical box containing many wild beasts. One may allow them to escape by opening the box's lid. "); break;
 case 8: strcat(descpr, "A deck of highly mysterious and magical cards. One may draw a random card from it, but should be prepared to suffer the possible consequences! "); break;
 case 9: strcat(descpr, "A deck of magical cards, depicting a range of weird and wondrous creatures. "); break;
 case 10: strcat(descpr, "A magical device which can be used to restore one's reserves of magical energy, but the use of which carries the risk of draining all of those energies completely. This risk varies inversely with the proportion of their maximum energy which the user possesses; a user near his or her full potential will find this item most beneficial. "); break;
 case 11: strcat(descpr, "A magical box after its power is spent. "); break;
 case 12: strcat(descpr, "A dangerous item which hypnotises anyone so unwise as to gaze into it, leaving them helpless for a significant length of time. "); break;
 case 13: strcat(descpr, "This extremely powerful item can unleash a destructive storm of electricity. It is especially effective in the hands of one skilled in air elemental magic, but cannot be used by one protected from its effects. "); break;
 case 14: strcat(descpr, "A talisman which allows entry into Zot's domain. "); break;
 case 15: strcat(descpr, "A deck of magical cards, full of amusing tricks. "); break;
 case 16: strcat(descpr, "A deck of powerful magical cards. "); break;
 case 17: strcat(descpr, "An altar to Nemelex Xobeh, built for easy assembly and disassembly. Invoke it to place on a clear patch of floor, then pick it up again when you've finished. "); break;
 }
break;


} // end switch ident_lev
break;

case 14: // corpses
if (item_type == 0)
{
strcat(descpr, "A corpse. ");
} else strcat(descpr, "A decaying skeleton. ");
break;


default:
strcat(descpr, "This item should not exist. Mayday! Mayday!");
break;

}

strcat(descpr, "It weighs around ");
char item_mass [7];
if (item_class == 14)
{
 if (item_type == 0) itoa(mons_weight(item_plus) / 10, item_mass, 10);
  else itoa(mons_weight(item_plus) / 20, item_mass, 10);
} else itoa(mass(item_class, item_type) / 10, item_mass, 10);
strcat(descpr, item_mass);
strcat(descpr, " aum. "); /* arbitrary unit of mass */

print_description(descpr);


if (getch() == 0) getch();

#ifdef DOS_TERM
puttext(25, 1, 80, 25, buffer);
window(1, 1, 80, 25);
#endif

} // end void describe_item

/*
Appends the various powers of a random artefact to the descpr string.
*/
void randart_descpr(char descpr [1000], int item_class, int item_type, int item_plus, int item_plus2, int item_dam)
{
 if (randart_wpn_properties(item_class, item_type, item_dam, item_plus, item_plus2, 0, RAP_AC) != 0)
 {
  strcat(descpr, "$It affects your AC (");
  append_value(descpr, randart_wpn_properties(item_class, item_type, item_dam, item_plus, item_plus2, 0, RAP_AC), 1);
  strcat(descpr, ").");
 }
 if (randart_wpn_properties(item_class, item_type, item_dam, item_plus, item_plus2, 0, RAP_EVASION) != 0)
 {
  strcat(descpr, "$It affects your evasion (");
  append_value(descpr, randart_wpn_properties(item_class, item_type, item_dam, item_plus, item_plus2, 0, RAP_EVASION), 1);
  strcat(descpr, ").");
 }
 if (randart_wpn_properties(item_class, item_type, item_dam, item_plus, item_plus2, 0, RAP_STRENGTH) != 0)
 {
  strcat(descpr, "$It affects your strength (");
  append_value(descpr, randart_wpn_properties(item_class, item_type, item_dam, item_plus, item_plus2, 0, RAP_STRENGTH), 1);
  strcat(descpr, ").");
 }
 if (randart_wpn_properties(item_class, item_type, item_dam, item_plus, item_plus2, 0, RAP_INTELLIGENCE) != 0)
 {
  strcat(descpr, "$It affects your intelligence (");
  append_value(descpr, randart_wpn_properties(item_class, item_type, item_dam, item_plus, item_plus2, 0, RAP_INTELLIGENCE), 1);
  strcat(descpr, ").");
 }
 if (randart_wpn_properties(item_class, item_type, item_dam, item_plus, item_plus2, 0, RAP_DEXTERITY) != 0)
 {
  strcat(descpr, "$It affects your dexterity (");
  append_value(descpr, randart_wpn_properties(item_class, item_type, item_dam, item_plus, item_plus2, 0, RAP_DEXTERITY), 1);
  strcat(descpr, ").");
 }
 if (randart_wpn_properties(item_class, item_type, item_dam, item_plus, item_plus2, 0, RAP_ACCURACY) != 0)
 {
  strcat(descpr, "$It affects your accuracy (");
  append_value(descpr, randart_wpn_properties(item_class, item_type, item_dam, item_plus, item_plus2, 0, RAP_ACCURACY), 1);
  strcat(descpr, ").");
 }
 if (randart_wpn_properties(item_class, item_type, item_dam, item_plus, item_plus2, 0, RAP_DAMAGE) != 0)
 {
  strcat(descpr, "$It affects your damage-dealing abilities (");
  append_value(descpr, randart_wpn_properties(item_class, item_type, item_dam, item_plus, item_plus2, 0, RAP_DAMAGE), 1);
  strcat(descpr, ").");
 }
 if (randart_wpn_properties(item_class, item_type, item_dam, item_plus, item_plus2, 0, RAP_FIRE) <= -1)
 {
  strcat(descpr, "$It makes you susceptible to fire. ");
 }
 if (randart_wpn_properties(item_class, item_type, item_dam, item_plus, item_plus2, 0, RAP_FIRE) == 1)
 {
  strcat(descpr, "$It protects you from fire. ");
 }
 if (randart_wpn_properties(item_class, item_type, item_dam, item_plus, item_plus2, 0, RAP_FIRE) == 2)
 {
  strcat(descpr, "$It renders you almost immune to fire. ");
 }
 if (randart_wpn_properties(item_class, item_type, item_dam, item_plus, item_plus2, 0, RAP_COLD) <= -1)
 {
  strcat(descpr, "$It makes you susceptible to cold. ");
 }
 if (randart_wpn_properties(item_class, item_type, item_dam, item_plus, item_plus2, 0, RAP_COLD) == 1)
 {
  strcat(descpr, "$It protects you from cold. ");
 }
 if (randart_wpn_properties(item_class, item_type, item_dam, item_plus, item_plus2, 0, RAP_COLD) == 2)
 {
  strcat(descpr, "$It renders you almost immune to cold. ");
 }
 if (randart_wpn_properties(item_class, item_type, item_dam, item_plus, item_plus2, 0, RAP_ELECTRICITY) != 0)
 {
  strcat(descpr, "$It insulates you from electricity. ");
 }
 if (randart_wpn_properties(item_class, item_type, item_dam, item_plus, item_plus2, 0, RAP_POISON) != 0)
 {
  strcat(descpr, "$It protects you from poison. ");
 }
 if (randart_wpn_properties(item_class, item_type, item_dam, item_plus, item_plus2, 0, RAP_NEGATIVE_ENERGY) != 0)
 {
  strcat(descpr, "$It protects you from negative energy. ");
 }
 if (randart_wpn_properties(item_class, item_type, item_dam, item_plus, item_plus2, 0, RAP_MAGIC) != 0)
 {
  strcat(descpr, "$It protects you from magic. ");
 }
 if (randart_wpn_properties(item_class, item_type, item_dam, item_plus, item_plus2, 0, RAP_EYESIGHT) != 0)
 {
  strcat(descpr, "$It enhances your eyesight. ");
 }
 if (randart_wpn_properties(item_class, item_type, item_dam, item_plus, item_plus2, 0, RAP_INVISIBLE) != 0)
 {
  strcat(descpr, "$It lets you turn invisible. ");
 }
 if (randart_wpn_properties(item_class, item_type, item_dam, item_plus, item_plus2, 0, RAP_LEVITATE) != 0)
 {
  strcat(descpr, "$It lets you levitate. ");
 }
 if (randart_wpn_properties(item_class, item_type, item_dam, item_plus, item_plus2, 0, RAP_BLINK) != 0)
 {
  strcat(descpr, "$It lets you blink. ");
 }
 if (randart_wpn_properties(item_class, item_type, item_dam, item_plus, item_plus2, 0, RAP_CAN_TELEPORT) != 0)
 {
  strcat(descpr, "$It lets you teleport. ");
 }
 if (randart_wpn_properties(item_class, item_type, item_dam, item_plus, item_plus2, 0, RAP_BERSERK) != 0)
 {
  strcat(descpr, "$It lets you go berserk. ");
 }
 if (randart_wpn_properties(item_class, item_type, item_dam, item_plus, item_plus2, 0, RAP_MAPPING) != 0)
 {
  strcat(descpr, "$It lets you sense your surroundings. ");
 }
 if (randart_wpn_properties(item_class, item_type, item_dam, item_plus, item_plus2, 0, RAP_NOISES) != 0)
 {
  strcat(descpr, "$It makes noises. ");
 }
 if (randart_wpn_properties(item_class, item_type, item_dam, item_plus, item_plus2, 0, RAP_PREVENT_SPELLCASTING) != 0)
 {
  strcat(descpr, "$It prevents spellcasting. ");
 }
 if (randart_wpn_properties(item_class, item_type, item_dam, item_plus, item_plus2, 0, RAP_CAUSE_TELEPORTATION) != 0)
 {
  strcat(descpr, "$It causes teleportation. ");
 }
 if (randart_wpn_properties(item_class, item_type, item_dam, item_plus, item_plus2, 0, RAP_PREVENT_TELEPORTATION) != 0)
 {
  strcat(descpr, "$It prevents most forms of teleportation. ");
 }
 if (randart_wpn_properties(item_class, item_type, item_dam, item_plus, item_plus2, 0, RAP_ANGRY) != 0)
 {
  strcat(descpr, "$It makes you angry. ");
 }
 if (randart_wpn_properties(item_class, item_type, item_dam, item_plus, item_plus2, 0, RAP_METABOLISM) != 0)
 {
  strcat(descpr, "$It speeds your metabolism. ");
 }
 if (randart_wpn_properties(item_class, item_type, item_dam, item_plus, item_plus2, 0, 25) != 0)
 {
  strcat(descpr, "$It emits mutagenic radiations, which may remain in your system for quite some time. ");
 }
 strcat(descpr, "$");
 if ((item_dam == 25 && (item_class == OBJ_WEAPONS || item_class == OBJ_ARMOUR)) || (item_dam == 201 && item_class == OBJ_JEWELLERY))
 {
  strcat(descpr, unrandart_descrip(0, item_class, item_type, item_plus, item_plus2));
  strcat(descpr, "$");
 }
}

/*
Used for weapons, armour and ammo.
*/
void print_ench(char descpr [1000], unsigned char item_plus)
{
 if (item_plus < 52) strcat(descpr, "lightly enchanted ");
 else if (item_plus < 54) strcat(descpr, "moderately enchanted ");
 else if (item_plus < 56) strcat(descpr, "heavily enchanted ");
 else if (item_plus < 58) strcat(descpr, "very heavily enchanted ");
 else strcat(descpr, "extremely heavily enchanted ");
}




/*
Describes every spell in the game.
*/
void describe_spell(int spelled)
{

   char descpr [1000];

#ifdef DOS_TERM
   char buffer[3400];
   gettext(25, 1, 80, 25, buffer);

   window(25, 1, 80, 25);
#endif
//   strcpy(st_pass, "");

   clrscr();

   strcpy(descpr, "");

strcat(descpr, "This spell");
switch(spelled)
{
case 0:
strcat(descpr, " allows the caster to determine the properties of an otherwise inscrutable magic item. ");
break;
case 1:
strcat(descpr, " teleports the caster to a random location. ");
break;
case 2:
strcat(descpr, " causes fear in those near to the caster. ");
break;
case 3:
strcat(descpr, " causes a loud noise to be heard. ");
break;
case 4:
strcat(descpr, " removes curses from any items which are being used by the caster. ");
break;
case 5:
strcat(descpr, " hurls a small bolt of magical energy. ");
break;
case 6:
strcat(descpr, " hurls an exploding bolt of fire. ");
break;
case 13:
strcat(descpr, " creates a column of roaring flame. ");
break;
case 14:
strcat(descpr, " digs a tunnel through unworked rock. ");
break;
case 15:
strcat(descpr, " hurls a great bolt of flames. ");
break;
case 16:
strcat(descpr, " hurls a great bolt of ice and frost. ");
break;
case 17:
strcat(descpr, " hurls a mighty bolt of lightning. Although this spell causes less damage than the similar fire and ice spells, it can rip through whole rows of creatures. ");
break;
case 20:
strcat(descpr, " randomly alters the form of another creature. ");
break;
case 21:
strcat(descpr, " slows the actions of a creature. ");
break;
case 22:
strcat(descpr, " speeds the actions of a creature. ");
break;
case 23:
strcat(descpr, " prevents a creature from moving. ");
break;
case 24:
strcat(descpr, " induces a state of bewilderment and confusion in a creature's mind. ");
break;
case 25:
strcat(descpr, " hides a creature from the sight of others. ");
break;
case 26:
strcat(descpr, " throws a small bolt of flame. ");
break;
case 27:
strcat(descpr, " throws a small bolt of frost. ");
break;
case 28:
strcat(descpr, " allows a short-range translocation, with precise control. ");
break;
case 29:
strcat(descpr, " conjures up a large cloud of lethally cold vapour. ");
break;
case 30:
strcat(descpr, " conjures up a large but short-lived cloud of vile fumes. ");
break;
case 31:
strcat(descpr, " surrounds the caster with a mobile ring of searing flame. It is not recommended for those vulnerable to the effects of fire. ");
break;
case 32:
strcat(descpr, " restores the physical strength of the caster. ");
break;
case 33:
strcat(descpr, " restores the intelligence of the caster. ");
break;
case 34:
strcat(descpr, " restores the dexterity of the caster. ");
break;
case 35:
strcat(descpr, " throws a bolt of deadly poison. ");
break;
case 36:
strcat(descpr, " bathes the caster's surroundings with a poisonous green light. ");
break;
case 37:
strcat(descpr, " randomly translocates another creature. ");
break;
case 38:
strcat(descpr, " heals a small amount of damage to the caster's body. ");
break;
case 39:
strcat(descpr, " heals a large amount of damage to the caster's body. ");
break;
case 40:
strcat(descpr, " removes poison from the caster's system. ");
break;
case 41:
strcat(descpr, " purifies the caster's body, removing poison, disease, and some malign enchantments. ");
break;
case 42:
//strcat(descpr, " is extremely powerful, but carries a degree of risk. It renders a living caster almost invulnerable to harm for a brief period of time, but brings them to the very brink of death in doing so. When the duration expires, the caster will be returned to a state of near-death. The spell can be cancelled at any time by any healing effect, and the caster will recieve one warning shortly before the spell expires. Undead cannot use this spell. ");
strcat(descpr, " is extremely powerful, but carries a degree of risk. It renders a living caster almost invulnerable to harm for a brief period of time, but can bring them dangerously close to death (how close depends on their necromantic abilities). The spell can be cancelled at any time by any healing effect, and the caster will recieve one warning shortly before the spell expires. Undead cannot use this spell. ");break;
case 43:
strcat(descpr, " allows the caster to selectively erase a spell from memory and regain the magical energy bound up in it. One will be able to memorise this spell even if their mind is otherwise full of magic (ie you have the maximum number of spells already; mere lack of spell levels is insufficient). ");
break;
case 44:
strcat(descpr, " causes confusion in all who see the caster. ");
break;
case 45:
strcat(descpr, " smites a creature of the caster's choice. ");
break;
case 46:
strcat(descpr, " calls on divine power to repel the unholy. ");
break;
case 47:
strcat(descpr, " involves the speaking of a word of power which repels and can destroy unholy creatures. ");
break;
case 48:
strcat(descpr, " alerts the caster to the presence of curses on his or her possessions. ");
break;
case 49:
strcat(descpr, " summons a small creature to the caster's aid. ");
break;
case 50:
strcat(descpr, " attempts to send hostile summoned creatures to the place from whence they came, or at least reduce the duration of their presence in the caster's locality. ");
break;
case 51:
strcat(descpr, " summons one or more giant scorpions to the caster's assistance. ");
break;
case 52:
strcat(descpr, " allows the caster to float in the air. ");
break;
case 53:
strcat(descpr, " hurls a deadly bolt of negative energy, which drains the life of any living creature which it strikes. ");
break;
case 54:
//strcat(descpr, " hurls a swarm of large and heavy metal splinters, to cut the caster's enemies into shreds. ");
strcat(descpr, " hurls a lethally sharp bolt of crystal. ");
break;
case 55:
strcat(descpr, " causes enormous damage to any creature struck by the bolt of incandescent energy that it conjures into existence. Unfortunately, it is very difficult to aim and very rarely hits anything. Pity, that. ");
break;
case 56:
strcat(descpr, " conjures forth a great cloud of lethal gasses. ");
break;
case 57:
strcat(descpr, " creates a mighty storm of roaring flame. ");
break;
case 58:
strcat(descpr, " detects any traps in the caster's vicinity. ");
break;
case 59:
strcat(descpr, " randomly translocates the caster a short distance. ");
break;
case 60:
strcat(descpr, " throws a crackling sphere of destructive energy. ");
break;
case 61:
strcat(descpr, " summons forth a pestilential swarm. ");
break;
case 62:
strcat(descpr, " opens a gate to the Abyss and calls through one or more hideous abominations from that dreadful place. The powers who supervise this invocation require the caster to surrender some of his or her intelligence in exchange for their service. ");
break;
case 63:
strcat(descpr, " causes an otherwise hostile creature to fight on your side for a while. ");
break;
case 64:
strcat(descpr, " reveals your surroundings. ");
break;
case 65:
strcat(descpr, " heals another creature from a distance. ");
break;
case 66:
strcat(descpr, " causes the dead to rise up and serve the caster; all corpses within a certain distance of the caster are affected. A powerful magician can create an army of mindless undead slaves with this spell. ");
break;
case 67:
strcat(descpr, " causes an extremely painful injury to a living creature. ");
break;
case 68:
strcat(descpr, " extends the durations of most beneficial enchantments affecting the caster. ");
break;
case 69:
strcat(descpr, " attempts to charm any undead in the vicinity of the caster. ");
break;
case 70:
strcat(descpr, " raises a skeleton to a state of unlife. ");
break;
case 71:
strcat(descpr, " steals the life of a living creature and grants it to the caster. It will not drain more life than the caster is capable of absorbing. ");
break;
case 72:
strcat(descpr, " calls on the powers of the undead to aid the caster. ");
break;
case 73:
strcat(descpr, " detects any items lying in the caster's general vicinity. ");
break;
case 74:
strcat(descpr, " instantly heals any and all wounds suffered by the caster, but permanently reduces their resilience by an amount dependent on their magical skill. ");
break;
case 75:
strcat(descpr, " burns a creature. ");
break;
case 76:
strcat(descpr, " freezes a creature. ");
break;
case 77:
strcat(descpr, " calls forth a spirit from the elemental planes to aid the caster. A large quantity of the desired element must be available; this is rarely a problem for earth and air, but may be for fire or water. The elemental will usually be friendly to the caster (especially if they are skilled in the appropriate form of elemental magic). ");
break;
case 78:
strcat(descpr, " drains the heat from the caster and the caster's surroundings, causing harm to all creatures not resistant to cold. ");
break;
case 79:
strcat(descpr, " conjures a sticky glob of liquid fire, which adheres to any creature it strikes and burns. ");
break;
case 80:
strcat(descpr, " calls forth a beast of ice to serve the caster. ");
break;
case 81:
strcat(descpr, " covers the caster's body with a protective layer of ice, the power of which depends on his or her skill with Ice magic. The caster and the caster's equipment are protected from the cold, but this spell will not function if the caster is already wearing heavy armour. ");
break;
case 82:
strcat(descpr, " calls forth a minor demon from the pits of Hell. ");
break;
case 83:
strcat(descpr, " reduces the chance of projectile attacks striking the caster. Even powerful attacks such as lightning bolts or dragon breath are affected, although smaller missiles are repelled to a much greater degree.");
break;
case 84:
strcat(descpr, " sends the caster into a temporary psychotic rage. ");
break;
case 85:
strcat(descpr, " inflicts a great deal of damage on an undead creature. ");
break;

case 110:
strcat(descpr, " allows its caster to imbue a mass of deceased flesh with a magical life force. Casting this spell involves the assembling of a heap of several corpses; the greater the mass of flesh available, the greater the chances of success. ");
break;
case 111:
strcat(descpr, " dramatically but temporarily increases the caster's recuperative abilities, while also increasing the rate at which he or she consumes food. ");
break;
case 112:
strcat(descpr, " uses the bones of a skeleton (or the bone-like exoskeleton of, for example, an insect) as the raw materials for a lethal spray of slicing bone fragments, allowing its creator to dispense with conjuration magic entirely and use necromancy alone for a low-level but very powerful combat spell. Using a large and heavy skeleton (by wielding it) results in a more powerful effect. ");
break;
case 113:
strcat(descpr, " banishes a creature to the Abyss. Magicians wishing to visit that unpleasant place may use it to banish themselves. ");
break;
case 114:
strcat(descpr, " mutates a creature into a pulsating mass of flesh. ");
break;
case 115:
strcat(descpr, " throws a magical dart of poison. ");
break;
case 116:
//strcat(descpr, " converts flesh, blood, and other bodily fluids into magical energy. The caster must hold a piece of butchered flesh in his or her hand when casting, or the spell will feed directly on the caster's body; although never directly fatal, this could leave one very near death. ");
strcat(descpr, " converts flesh, blood, and other bodily fluids into magical energy. The caster may use his or her own body as the focus for this spell (which can be dangerous but never directly lethal), or can wield a piece of butchered flesh and draw power from that. ");
break;
case 117:
strcat(descpr, " causes a weapon held in the caster's hand to dance into the air and strike the caster's enemies. It will not function on magical staves or certain items of a unique nature. ");
break;
case 118:
strcat(descpr, " should only be available from Dispater's staff. So how are you reading this? ");
break;
case 119:
strcat(descpr, " opens a gate to the realm of Pandemonium and draws forth one of its inhabitants to serve the caster for a time. ");
break;
case 120:
strcat(descpr, " draws a small swarm of small demons to do battle with the caster's foes. ");
break;
case 121:
strcat(descpr, " calls one of the greater demons of Pandemonium forth to serve the caster. Beware, for the spell binding it to service may not outlast the spell binding it to this world! ");
break;
case 122:
strcat(descpr, " rapidly accelerates the process of decay in any corpses lying around the caster, in the process emitting a foul miasmic vapour which eats away at the life force of any creature within it. ");
break;
case 123:
strcat(descpr, " bestows a lethal but temporary sharpness on a sword held by the caster. It will not affect weapons which are otherwise affected by special enchantments. ");
break;
case 124:
strcat(descpr, " sets a weapon held by the caster on fire. It will not affect weapons which are otherwise affected by special enchantments. ");
break;
case 125:
strcat(descpr, " infuses a weapon held by the caster with an aura of freezing cold. It will not affect weapons which are otherwise affected by special enchantments. ");
break;
case 126:
strcat(descpr, " infuses a weapon held by the caster with unholy energies. It will not affect weapons which are otherwise affected by special enchantments. ");
break;
case 127:
strcat(descpr, " crushes a nearby creature with waves of gravitational force. ");
break;
case 128:
strcat(descpr, " hurls a large and heavy metal bolt at the caster's foes. ");
break;
case 129:
strcat(descpr, " hurls a sharp spine of rock. ");
break;
case 130:
strcat(descpr, " entombs the caster within four walls of rock. These walls will destroy most objects in their way, but their growth is obstructed by the presence of any creatures. Beware - one would be unwise to use this spell without a reliable escape route. ");
break;
case 131:
strcat(descpr, " protects the caster with chunky scales of stone, the power of which depends on his or her Earth magic skill. These scales can coexist with other forms of armour, but are extremely heavy and cumbersome. ");
break;
case 132:
strcat(descpr, " throws a bolt of electricity. ");
break;
case 133:
strcat(descpr, " imbues its caster with the ability to run extremely fast. It will not increase the speed of those who are fleet of foot anyway (such as centaurs). ");
break;
case 134:
strcat(descpr, " grants its caster the ability to fly through the air. ");
break;
case 135:
strcat(descpr, " protects its caster from electric shocks. ");
break;
case 136:
strcat(descpr, " hurls a crackling orb of electrical energy which causes a huge explosion on impact. ");
break;
case 137:
strcat(descpr, " allows the caster to detect any creatures within a certain radius. ");
break;
case 138:
strcat(descpr, " removes some or all of any poisons from the caster's system. ");
break;
case 139:
strcat(descpr, " allows the caster to control his or her translocations. ");
break;
case 140:
strcat(descpr, " poisons missile ammunition held by the caster. ");
break;
case 141:
strcat(descpr, " covers a sharp bladed weapon with poison for a while. ");
break;
case 142:
strcat(descpr, " protects your body from any poisons which may enter it for a period of time. ");
break;
case 143:
strcat(descpr, " produces a noise emanating from a place of the caster's choice. ");
break;
case 144:
strcat(descpr, " causes a weird alteration in the caster's body, leaving the caster in a weakened state (but never killing them). It may fail to affect those who are heavily mutated already. ");
break;
/* 145 - debugging ray */
case 146:
strcat(descpr, " is greatly prized by summoners and necromancers, as it allows the caster to recall any friendly creatures who may be nearby to a position adjacent to the caster.");
break;
case 147:
strcat(descpr, " creates a gate allowing long-distance travel through a relatively ordinary environment (ie the Dungeon only). It lasts long enough for the caster and nearby creatures to enter. It will not take you past the level limits of your current area.");
break;
case 148:
strcat(descpr, " cuts the resilience of a target creature in half, although it will never cause death directly.");
break;
case 149:
strcat(descpr, " temporarily transforms the caster into a venomous spider-like creature.");
break;
case 150:
strcat(descpr, " disrupts the matter of a creature's body, causing injury.");
break;
case 151:
strcat(descpr, " tears apart the matter making up an object (especially a creature's body), causing severe damage.");
break;
case 152:
strcat(descpr, " causes long, scythe-shaped blades to grow from the caster's hands. It makes spellcasting a little difficult.");
break;
case 153:
strcat(descpr, " temporarily transforms the caster into a slow-moving but extremely robust stone statue.");
break;
case 154:
strcat(descpr, " temporarily transform's the caster's body into a frozen ice-creature.");
break;
case 155:
strcat(descpr, " temporarily transforms the caster into a great fire-breathing dragon.");
break;
case 156:
strcat(descpr, " first transforms the caster into a semi-corporeal apparition receptive to negative energy, then infuses that form with the powers of Death. The caster becomes resistant to cold, poison, magic and hostile negative energies.");
break;
case 157:
strcat(descpr, " raises living creatures slain by the caster into a state of unliving slavery as spectral horrors.");
break;
case 158:
strcat(descpr, " calls on the powers of Hell to cause agonising injuries to anyone or anything living in the caster's vicinity. It carries a degree of danger for any necromancer brave enough to invoke it, for the Symbol also affects its caller and indeed will not function if they are immune to its terrible effects. Despite its power, however, it is never lethal.");
break;
case 159:
strcat(descpr, " protects the caster from any kind of projectile attack, although particular powerful attacks (lightning bolts, etcetera) are deflected less than lighter missiles. ");
break;
case SPELL_ORB_OF_FRAGMENTATION:
strcat(descpr, " throws a heavy sphere of metal which explodes on impact into a blast of deadly jagged fragments. It can rip a creature to shreds, but is ineffective against heavily armoured targets.");
break;
case SPELL_ICE_BOLT:
strcat(descpr, " throws a chunk of ice. It is particularly effective against those creatures not immune to the effects of freezing temperatures, but half of its destructive potential comes from its weight and sharp edges and cannot be ignored even by cold-resistant creatures.");
break;
case SPELL_ICE_STORM:
strcat(descpr, " conjures forth a raging blizzard of ice, sleet and freezing gasses.");
break;
case SPELL_ARC:
strcat(descpr, " zaps a nearby creature with a powerful electrical current.");
break;
case SPELL_AIRSTRIKE:
strcat(descpr, " causes the air around a creature to twist itself into a whirling vortex and attack.");
break;
case SPELL_SHADOW_CREATURES:
strcat(descpr, " weaves a creature from shadows and threads of Abyssal matter. The creature thus brought into existence will be a recreation of some type of creature found in the caster's immediate vicinity. The spell even creates appropriate pieces of equipment for the creature, which are given a lasting substance by their contact with firm reality.");
break;

/*
case 0:
strcat(descpr, ". ");
break;
*/
default:
strcpy(descpr, "This spell apparently does not exist. Casting it may therefore be unwise. ");
break;

}

print_description(descpr);


if (getch() == 0) getch();

#ifdef DOS_TERM
puttext(25, 1, 80, 25, buffer);
window(1, 1, 80, 25);
#endif

} // end void describe_item

/*
Describes all gods. Accessible through altars (by praying), or by the ^ key
if player is a worshipper.
*/
void describe_god(int which_god)
{

   char descpr [1000];

#ifdef DOS_TERM
   char buffer[3400];
   gettext(25, 1, 80, 25, buffer);

   window(25, 1, 80, 25);
#endif
//   strcpy(st_pass, "");

   clrscr();

   strcpy(descpr, "");

switch(which_god)
{
case 1:
strcpy(descpr, "Zin is an ancient and revered God, dedicated to the \
establishment of order and the destruction of the forces of chaos and night. \
Valued \
worshippers can gain a variety of powers useful in the fight against evil, \
but must abstain from the use of necromancy and other forms of unholy magic. \
Zin appreciates long-standing faith as well as sacrifices of valued objects \
and the slaying of demons and the undead.");
break;
case 2:
strcpy(descpr, "The Shining One is a powerful crusading diety, allied with \
Zin in the fight against evil. Followers may be granted with the ability to \
summarily dispense the wrath of heaven, but must never use any form of evil \
magic and should fight honourably. The Shining One appreciates long-standing persistence in the endless \
crusade, as well as the dedicated destruction of unholy creatures.");
break;
case 3:
strcpy(descpr, "Kikubaaqudgha is a terrible Demon-God, served by those who \
seek knowledge of the powers of death. Followers gain special powers \
over the undead, and especially favoured servants can call on mighty demons \
to slay their foes. $Kikubaaqudgha requires the deaths of living creatures \
as often as possible, but is not interested in the offering of corpses except \
 at an appropriate altar.");
if (you[0].piety >= 50 && you[0].religion == GOD_KIKUBAAQUDGHA)
 strcat(descpr, "$Kikubaaqudgha is protecting you from the side-effects of death magic.");
break;
case 4:
strcpy(descpr, "Yredelemnul is worshipped by those who seek powers over \
death and the undead without having to learn to use necromancy. Followers \
can raise legions of servile undead and gain a number of other useful (if \
unpleasant) powers. Yredelemnul appreciates killing, but prefers corpses \
to be put to use rather than sacrificed.");
break;
case 5:
strcpy(descpr, "Xom is a wild and unpredictable God of chaos, who seeks not \
worshippers but playthings to toy with. Many choose to follow Xom in the \
hope of receiving fabulous rewards and mighty powers, but Xom is nothing if \
not capricious.");
break;
case 6:
strcpy(descpr, "Vehumet is a God of the destructive powers of magic. \
Followers gain various useful powers to enhance their command of \
the hermetic arts, and the most favoured stand to gain access to \
some of the fearsome spells in Vehumet's library. One's devotion to Vehumet \
can be proved by the causing of as much carnage and destruction as possible.");
if (you[0].religion == GOD_VEHUMET)
{
 if (you[0].piety >= 30)
  strcat(descpr, "$You can gain power from the those you kill in Vehumet's name, or those slain by your servants.");
 if (you[0].piety >= 50)
  strcat(descpr, "$Your conjurations and summonings cast in Vehumet's name rarely fail.");
 if (you[0].piety >= 75)
  strcat(descpr, "$During prayer you are protected from summoned creatures.");
}
break;
case 7:
strcpy(descpr, "Okawaru is a dangerous and powerful God of battle. Followers \
can gain a number of powers useful in combat as well as various rewards, \
but must constantly prove \
themselves through battle and the sacrifice of corpses and valuable items.");
break;
case 8:
strcpy(descpr, "Makhleb the Destroyer is a fearsome God of chaos and violent \
death. Followers, who must constantly appease Makhleb with blood, stand to \
gain various powers of death and destruction. The Destroyer appreciates \
sacrifices of corpses and valuable items.");
if (you[0].piety >= 30 && you[0].religion == GOD_MAKHLEB)
 strcat(descpr, "$You can gain power from the deaths of those killed in Makhleb's name.");
break;
case 9:
strcpy(descpr, "Sif Muna is a contemplative but powerful deity, served by \
those who seek magical knowledge. Sif Muna appreciates sacrifices of valuable \
items, and the casting of spells as often as possible.");
if (you[0].piety >= 100 && you[0].religion == GOD_SIF_MUNA)
 strcat(descpr, "$Sif Muna is protecting you from some of the side-effects of magic.");
break;
case 10:
strcpy(descpr, "Trog is an ancient God of anger and violence. Followers are \
expected to kill in Trog's name and sacrifice the dead, and in return gain \
power in battle and occasional rewards. Trog hates wizards, and followers \
are forbidden the use of spell magic.");
break;
case 11:
strcpy(descpr, "Nemelex is a strange and unpredictable trickster God, whose \
powers can be invoked through the magical packs of cards which Nemelex \
paints in the ichor of demons. Followers receive occasional gifts, and \
should use these gifts as much as possible. Offerings of items are also \
appreciated.");
break;
case 12:
strcpy(descpr, "Elyvilon the Healer is worshipped by the healers (among \
others), who gain their healing powers by long worship and devotion. Although Elyvilon \
prefers a creed of pacifism, those who crusade against evil are not \
excluded. Elyvilon appreciates the offering of weapons.");
break;

}


print_description(descpr);


if (getch() == 0) getch();

#ifdef DOS_TERM
puttext(25, 1, 80, 25, buffer);
window(1, 1, 80, 25);
#endif


}



/*
Takes a descpr string filled up with stuff from other functions, and
displays it with minor formatting to avoid cut-offs in mid word and such.
The character $ is interpreted as a CR.
*/
void print_description(char descr [1000])
{

unsigned int i = 0;
int j = 0;

textcolor(7);

//char descrpr [250];
//char kl [2];

//strcpy(kl, "");

for (i = 0; i < strlen(descr); i ++)
{

//if (descr [i] != 32) continue;

//for (i = 0; i < strlen(descr); i ++)
//{
// if (descr [j] != 32) continue;
// if (j %
//}
//kl = descr [i];
//cprintf(descr);
//break;
//puts(kl);
//strcpy(kl, "");
//kl [0] = descr [i];
//cprintf(kl);
if (descr [i] == '$')
{
#ifdef DOS_TERM
  cprintf(EOL);
#endif
#ifdef PLAIN_TERM
  gotoxy(1, wherey() + 1);
#endif
 j = 0;
} else putch(descr [i]);
#ifdef DOS_TERM
if (j % 55 >= 43 && descr [i] == 32 && descr [i + 1] != '$')
#endif
#ifdef PLAIN_TERM
if (j % 70 >= 58 && descr [i] == 32 && descr [i + 1] != '$')
#endif
{
#ifdef DOS_TERM
  cprintf(EOL);
#endif
#ifdef PLAIN_TERM
  gotoxy(1, wherey() + 1);
#endif
 j = 0;
}
j ++;
}

}



/*
Appends a value to the string. If plussed == 1, will add a + to positive
values (itoa always adds - to -ve ones).
*/
void append_value(char descpr [1000], int valu, char plussed)
{
char value_str [5];
if (valu >= 0 && plussed == 1) strcat(descpr, "+");
itoa(valu, value_str, 10);
strcat(descpr, value_str);
}



/* Describes the random demons you find in Pandemonium */
void describe_demon(char descpr [1000])
{

long globby = 0;
long randstore = random();
unsigned int i = 0;

for (i = 0; i < strlen(ghost.gname); i ++)
{
 globby += ghost.gname [i];
}

globby *= strlen(ghost.gname);

srandom(globby);

strcpy(descpr, "A powerful demon, ");
strcat(descpr, ghost.gname);
strcat(descpr, " has a");

switch(random2(31))
{
case 0: strcat(descpr, " huge, barrel-shaped "); break;
case 1: strcat(descpr, " wispy, insubstantial "); break;
case 2: strcat(descpr, " spindly "); break;
case 3: strcat(descpr, " skeletal "); break;
case 4: strcat(descpr, " horribly deformed "); break;
case 5: strcat(descpr, " spiny "); break;
case 6: strcat(descpr, " waif-like "); break;
case 7: strcat(descpr, " scaly "); break;
case 8: strcat(descpr, " sickeningly deformed "); break;
case 9: strcat(descpr, " bruised and bleeding "); break;
case 10: strcat(descpr, " sickly "); break;
case 11: strcat(descpr, " mass of writhing tentacles for a "); break;
case 12: strcat(descpr, " mass of ropey tendrils for a "); break;
case 13: strcat(descpr, " tree trunk-like "); break;
case 14: strcat(descpr, " hairy "); break;
case 15: strcat(descpr, " furry "); break;
case 16: strcat(descpr, " fuzzy "); break;
case 17: strcat(descpr, "n obese "); break;
case 18: strcat(descpr, " fat "); break;
case 19: strcat(descpr, " slimey "); break;
case 20: strcat(descpr, " wrinkled "); break;
case 21: strcat(descpr, " metallic "); break;
case 22: strcat(descpr, " glassy "); break;
case 23: strcat(descpr, " crystalline "); break;
case 24: strcat(descpr, " muscular "); break;
case 25: strcat(descpr, "n icky "); break;
case 26: strcat(descpr, " swollen "); break;
case 27: strcat(descpr, " lumpy "); break;
case 28: strcat(descpr, " armoured "); break;
case 29: strcat(descpr, " carapaced "); break;
case 30: strcat(descpr, " slender "); break;
}

strcat(descpr, "body");

switch(random2(40))
{
 case 0: if (ghost.ghs [10] == 1) strcat(descpr, " with small insectoid wings"); break;
 case 1: if (ghost.ghs [10] == 1) strcat(descpr, " with large insectoid wings"); break;
 case 2: if (ghost.ghs [10] == 1) strcat(descpr, " with moth-like wings"); break;
 case 3: if (ghost.ghs [10] == 1) strcat(descpr, " with buttefly wings"); break;
 case 4: if (ghost.ghs [10] == 1) strcat(descpr, " with huge, bat-like wings"); break;
 case 5: if (ghost.ghs [10] == 1) strcat(descpr, " with fleshy wings"); break;
 case 6: if (ghost.ghs [10] == 1) strcat(descpr, " with small, bat-like wings"); break;
 case 7: if (ghost.ghs [10] == 1) strcat(descpr, " with hairy wings"); break;
 case 8: if (ghost.ghs [10] == 1) strcat(descpr, " with great feathered wings"); break;
 case 9: if (ghost.ghs [10] == 1) strcat(descpr, " with shiny metal wings"); break;
 case 10: if (ghost.ghs [10] == 2) strcat(descpr, " which hovers in mid-air"); break;
 case 11: if (ghost.ghs [10] == 2) strcat(descpr, " with sacs of gas hanging from its back"); break;
 case 12: strcat(descpr, " covered in tiny crawling spiders"); break;
 case 13: strcat(descpr, " covered in tiny crawling insects"); break;
 case 14: strcat(descpr, " and the head of a crocodile"); break;
 case 15: strcat(descpr, " and the head of a hippopotamous"); break;
 case 16: strcat(descpr, " and a cruel curved beak for a mouth"); break;
 case 17: strcat(descpr, " and a straight sharp beak for a mouth"); break;
 case 18: strcat(descpr, " and no head at all"); break;
 case 19: strcat(descpr, " and a hideous tangle of tentacles for a mouth"); break;
 case 20: strcat(descpr, " and an elephantine trunk"); break;
 case 21: strcat(descpr, " and an evil-looking proboscis"); break;
 case 22: strcat(descpr, " and dozens of eyes"); break;
 case 23: strcat(descpr, " and two ugly heads"); break;
 case 24: strcat(descpr, " and a long serpentine tail"); break;
 case 25: strcat(descpr, " and a pair of huge tusks growing from its jaw"); break;
 case 26: strcat(descpr, " and a single huge eye, in the centre of its forehead"); break;
 case 27: strcat(descpr, " and spikes of black metal for teeth"); break;
 case 28: strcat(descpr, " and a disc-shaped sucker for a head"); break;
 case 29: strcat(descpr, " and huge, flapping ears"); break;
 case 30: strcat(descpr, " and a huge, toothy maw in the centre of its chest"); break;
 case 31: strcat(descpr, " and a giant snailshell on its back"); break;
 case 32: strcat(descpr, " and a dozen heads"); break;
 case 33: strcat(descpr, " and the head of a jackal"); break;
 case 34: strcat(descpr, " and the head of a baboon"); break;
 case 35: strcat(descpr, " and a huge, slobbery tongue"); break;
 case 36: strcat(descpr, " which is covered in oozing lacerations"); break;
 case 37: strcat(descpr, " and the head of a frog"); break;
 case 38: strcat(descpr, " and the head of a yak"); break;
 case 39: strcat(descpr, " and eyes out on stalks"); break;
}
/* Often won't get anything appended there. */

strcat(descpr, ".");

switch(random2(40))
{
 case 0: strcat(descpr, " It radiates an aura of extreme power."); break;
 case 1: strcat(descpr, " It is surrounded by a sickening stench."); break;
 case 2: strcat(descpr, " It is surrounded by a brilliant glow"); break;
 case 3: strcat(descpr, " It radiates an aura of extreme hatred for all that lives."); break;
 case 4: strcat(descpr, " Tiny orange flames dance around it."); break;
 case 5: strcat(descpr, " Tiny purple flames dance around it."); break;
 case 6: strcat(descpr, " It is surrounded by a weird haze."); break;
 case 7: strcat(descpr, " It glows with a malevolent light."); break;
 case 8: strcat(descpr, " It looks incredibly angry."); break;
 case 9: strcat(descpr, " It oozes with slime."); break;
 case 10: strcat(descpr, " It dribbles constantly."); break;
 case 11: strcat(descpr, " Mould grows all over it."); break;
 case 12: strcat(descpr, " It looks diseased."); break;
 case 13: strcat(descpr, " It looks as frightened of you as you are of it."); break;
 case 14: strcat(descpr, " It stinks."); break;
 case 15: strcat(descpr, " It moves in a serious of hideous convulsions."); break;
 case 16: strcat(descpr, " It moves with an unearthly grace."); break;
 case 17: strcat(descpr, " It hungers for your soul!"); break;
 case 18: strcat(descpr, " Its joints are swollen as if afflicted by arthritis."); break;
 case 19: strcat(descpr, " smells like rotting flesh."); break;
 case 20: strcat(descpr, " shimmers before your eyes."); break;
}

srandom(randstore);

}

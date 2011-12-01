# Makefile chooser.  Choose one:

MAKEFILE = makefile.lnx
#MAKEFILE = makefile.sgi
#MAKEFILE = makefile.dos
#MAKEFILE = makefile.emx
#MAKEFILE = makefile.sol

#jmf: number of concurrent jobs -- good value is (#_of_CPUs * 2) + 1
#     cuts build time a lot on multi-cpu machines
# OTHER=-j2
OTHER=

target: all

all:
	$(MAKE) $(OTHER) -f $(MAKEFILE) EXTRA_FLAGS='-O2 -fno-strength-reduce'

noopt:
	$(MAKE) $(OTHER) -f $(MAKEFILE)

install:
	$(MAKE) $(OTHER) -f $(MAKEFILE) install

clean:
	$(MAKE) $(OTHER) -f $(MAKEFILE) clean

distclean:
	$(MAKE) $(OTHER) -f $(MAKEFILE) distclean

# WIZARD mode currently includes asserts, bounds checking, and item checking
wizard:
	$(MAKE) $(OTHER) -f $(MAKEFILE) debug EXTRA_FLAGS='-g -DWIZARD -DDEBUG -DDEBUG_ITEM_SCAN'

# DEBUG mode includes WIZARD mode as well as copious debugging input
debug:
	$(MAKE) $(OTHER) -f $(MAKEFILE) debug EXTRA_FLAGS='-g -DFULLDEBUG -DWIZARD'

profile:
	$(MAKE) $(OTHER) -f $(MAKEFILE) EXTRA_FLAGS='-pg -O2 -fno-strength-reduce'

skilldump:
	$(MAKE) $(OTHER) -f $(MAKEFILE) debug EXTRA_FLAGS='-g -DDEBUG_DUMP_SKILL_TOTALS'

# DO NOT DELETE THIS LINE -- $(MAKE) depend depends on it.

Kills.o: AppHdr.h libutil.h chardump.h describe.h externs.h
Kills.o: defines.h enum.h FixAry.h FixVec.h
Kills.o: debug.h Kills.h
Kills.o: message.h mon-util.h files.h globals.h itemname.h travel.h tags.h
MacString.o: AppHdr.h libutil.h MacString.h
abl-show.o: AppHdr.h libutil.h abl-show.h enum.h
abl-show.o: globals.h defines.h externs.h
abl-show.o: FixAry.h FixVec.h
abl-show.o: debug.h Kills.h message.h beam.h effects.h food.h it_use2.h
abl-show.o: macro.h misc.h monplace.h player.h religion.h skills.h skills2.h
abl-show.o: spl-cast.h spl-util.h direct.h spells1.h spells2.h spells3.h
abl-show.o: spells4.h stuff.h transfor.h view.h
abyss.o: AppHdr.h libutil.h abyss.h
abyss.o: globals.h defines.h externs.h
abyss.o: enum.h FixAry.h FixVec.h debug.h
abyss.o: Kills.h
abyss.o: message.h cloud.h monplace.h dungeon.h items.h mon-util.h monstuff.h
abyss.o: lev-pand.h randart.h stuff.h view.h
acr.o: AppHdr.h libutil.h
acr.o: globals.h defines.h
acr.o: externs.h enum.h FixAry.h FixVec.h debug.h Kills.h message.h
acr.o: abl-show.h abyss.h chardump.h cloud.h command.h decks.h delay.h
acr.o: describe.h direct.h dungeon.h effects.h fight.h files.h food.h
acr.o: hiscores.h initfile.h invent.h menu.h item_use.h it_use2.h it_use3.h
acr.o: itemname.h itemprop.h items.h lev-pand.h macro.h misc.h monplace.h
acr.o: monstuff.h mon-util.h mutation.h newgame.h ouch.h output.h overmap.h
acr.o: player.h randart.h religion.h shopping.h skills.h skills2.h spells1.h
acr.o: spells2.h spells3.h spells4.h spl-book.h spl-cast.h spl-util.h stuff.h
acr.o: tags.h transfor.h travel.h stash.h view.h
beam.o: AppHdr.h libutil.h beam.h externs.h
beam.o: defines.h enum.h FixAry.h FixVec.h
beam.o: debug.h Kills.h
beam.o: message.h globals.h
beam.o: cloud.h delay.h effects.h fight.h it_use2.h itemname.h itemprop.h
beam.o: items.h misc.h monplace.h monstuff.h mon-util.h mstuff2.h ouch.h
beam.o: player.h religion.h skills.h skills2.h spells1.h direct.h spells3.h
beam.o: spells4.h stuff.h view.h
chardump.o: AppHdr.h libutil.h chardump.h
chardump.o: globals.h
chardump.o: defines.h externs.h enum.h FixAry.h FixVec.h debug.h Kills.h
chardump.o: message.h describe.h fight.h itemname.h itemprop.h items.h
chardump.o: macro.h mutation.h output.h player.h religion.h shopping.h
chardump.o: skills2.h spl-book.h spl-cast.h spl-util.h direct.h stash.h
chardump.o: stuff.h version.h view.h
cloud.o: AppHdr.h libutil.h globals.h defines.h externs.h
cloud.o: enum.h FixAry.h FixVec.h debug.h
cloud.o: Kills.h
cloud.o: message.h cloud.h stuff.h ouch.h misc.h monstuff.h mon-util.h
command.o: AppHdr.h libutil.h command.h
command.o: globals.h defines.h externs.h
command.o: enum.h FixAry.h
command.o: FixVec.h debug.h Kills.h message.h abl-show.h food.h invent.h
command.o: menu.h itemname.h itemprop.h item_use.h items.h ouch.h player.h
command.o: spl-cast.h spl-util.h direct.h stuff.h version.h
debug.o: AppHdr.h libutil.h debug.h
debug.o: globals.h defines.h externs.h
debug.o: enum.h FixAry.h
debug.o: FixVec.h Kills.h message.h chardump.h direct.h dungeon.h files.h
debug.o: invent.h menu.h itemname.h itemprop.h items.h misc.h monplace.h
debug.o: monstuff.h mon-util.h mutation.h newgame.h player.h randart.h
debug.o: religion.h skills.h skills2.h spl-cast.h spl-util.h stuff.h
decks.o: AppHdr.h libutil.h decks.h
decks.o: globals.h defines.h externs.h
decks.o: enum.h FixAry.h
decks.o: FixVec.h debug.h Kills.h message.h effects.h food.h it_use2.h
decks.o: items.h itemprop.h misc.h monplace.h mutation.h ouch.h player.h
decks.o: religion.h spells1.h direct.h spells3.h spells4.h spl-cast.h stuff.h
delay.o: AppHdr.h libutil.h globals.h defines.h externs.h
delay.o: enum.h FixAry.h FixVec.h debug.h
delay.o: Kills.h
delay.o: message.h delay.h effects.h fight.h food.h
delay.o: items.h itemname.h itemprop.h item_use.h it_use2.h misc.h monstuff.h
delay.o: mstuff2.h ouch.h output.h player.h randart.h religion.h spl-cast.h
delay.o: spl-util.h direct.h stuff.h transfor.h view.h
describe.o: AppHdr.h libutil.h describe.h externs.h
describe.o: defines.h enum.h FixAry.h FixVec.h
describe.o: debug.h Kills.h
describe.o: message.h
describe.o: globals.h
describe.o: abl-show.h fight.h initfile.h itemname.h itemprop.h macro.h
describe.o: misc.h mon-util.h player.h randart.h religion.h shopping.h
describe.o: skills2.h stuff.h spl-util.h direct.h
direct.o: AppHdr.h libutil.h direct.h externs.h
direct.o: defines.h enum.h FixAry.h FixVec.h
direct.o: debug.h Kills.h
direct.o: message.h globals.h
direct.o: cloud.h describe.h itemname.h misc.h monstuff.h mon-util.h player.h
direct.o: shopping.h stuff.h spells4.h view.h macro.h
dungeon.o: AppHdr.h libutil.h abyss.h defines.h
dungeon.o: enum.h globals.h externs.h FixAry.h FixVec.h debug.h Kills.h
dungeon.o: message.h dungeon.h itemname.h itemprop.h items.h maps.h misc.h
dungeon.o: mon-util.h mon-pick.h monplace.h player.h randart.h spl-book.h
dungeon.o: stuff.h
effects.o: AppHdr.h libutil.h effects.h externs.h
effects.o: defines.h enum.h FixAry.h FixVec.h
effects.o: debug.h Kills.h
effects.o: message.h
effects.o: globals.h abyss.h
effects.o: beam.h delay.h direct.h dungeon.h itemname.h itemprop.h items.h
effects.o: misc.h monplace.h monstuff.h mon-util.h mutation.h newgame.h
effects.o: ouch.h player.h randart.h skills2.h spells3.h spells4.h spl-book.h
effects.o: spl-util.h stuff.h view.h
fight.o: AppHdr.h libutil.h fight.h externs.h
fight.o: defines.h enum.h FixAry.h FixVec.h
fight.o: debug.h Kills.h
fight.o: message.h
fight.o: globals.h beam.h cloud.h delay.h
fight.o: effects.h food.h it_use2.h items.h itemname.h itemprop.h macro.h
fight.o: misc.h monplace.h mon-pick.h monstuff.h mon-util.h mstuff2.h
fight.o: mutation.h ouch.h player.h randart.h religion.h skills.h skills2.h
fight.o: spells1.h direct.h spells3.h spells4.h spl-cast.h stuff.h transfor.h
fight.o: view.h
files.o: AppHdr.h libutil.h files.h FixAry.h FixVec.h debug.h
files.o: globals.h defines.h
files.o: externs.h enum.h
files.o: Kills.h message.h cloud.h dungeon.h itemname.h itemprop.h items.h
files.o: misc.h monstuff.h mon-util.h mstuff2.h player.h randart.h skills2.h
files.o: stash.h stuff.h tags.h travel.h view.h
food.o: AppHdr.h libutil.h food.h
food.o: globals.h defines.h externs.h
food.o: enum.h FixAry.h FixVec.h debug.h Kills.h
food.o: message.h delay.h effects.h invent.h menu.h items.h itemname.h
food.o: itemprop.h item_use.h it_use2.h macro.h misc.h mon-util.h mutation.h
food.o: player.h religion.h skills2.h stuff.h
globals.o: AppHdr.h libutil.h defines.h globals.h externs.h
globals.o: enum.h
globals.o: FixAry.h FixVec.h debug.h Kills.h
globals.o: message.h
hiscores.o: AppHdr.h
hiscores.o: libutil.h globals.h defines.h externs.h
hiscores.o: enum.h FixAry.h FixVec.h debug.h Kills.h
hiscores.o: message.h hiscores.h itemname.h mon-util.h player.h religion.h
hiscores.o: stuff.h tags.h view.h skills2.h
initfile.o: AppHdr.h libutil.h initfile.h
initfile.o: Kills.h globals.h defines.h externs.h
initfile.o: enum.h FixAry.h
initfile.o: FixVec.h debug.h message.h misc.h player.h stash.h stuff.h
initfile.o: travel.h items.h view.h
insult.o: AppHdr.h libutil.h
insult.o: globals.h defines.h externs.h
insult.o: enum.h FixAry.h
insult.o: FixVec.h debug.h Kills.h message.h insult.h mon-util.h stuff.h
invent.o: AppHdr.h libutil.h invent.h menu.h defines.h
invent.o: globals.h externs.h
invent.o: enum.h FixAry.h FixVec.h debug.h Kills.h
invent.o: message.h itemname.h items.h itemprop.h macro.h player.h shopping.h
invent.o: stuff.h view.h
it_use2.o: AppHdr.h libutil.h it_use2.h externs.h
it_use2.o: defines.h enum.h FixAry.h FixVec.h
it_use2.o: debug.h Kills.h
it_use2.o: message.h globals.h beam.h effects.h food.h
it_use2.o: itemname.h itemprop.h misc.h mutation.h player.h randart.h
it_use2.o: religion.h skills2.h spells2.h spl-cast.h stuff.h view.h
it_use3.o: AppHdr.h libutil.h it_use3.h
it_use3.o: globals.h defines.h externs.h
it_use3.o: enum.h FixAry.h FixVec.h debug.h Kills.h
it_use3.o: message.h beam.h decks.h direct.h effects.h fight.h food.h items.h
it_use3.o: it_use2.h itemname.h itemprop.h misc.h monplace.h monstuff.h
it_use3.o: mon-util.h player.h randart.h religion.h skills.h skills2.h
it_use3.o: spells1.h spells2.h spl-book.h spl-cast.h spl-util.h stuff.h
it_use3.o: view.h
item_use.o: AppHdr.h libutil.h item_use.h
item_use.o: globals.h defines.h externs.h
item_use.o: enum.h FixAry.h
item_use.o: FixVec.h debug.h Kills.h message.h beam.h delay.h describe.h
item_use.o: direct.h effects.h fight.h food.h invent.h menu.h it_use2.h
item_use.o: it_use3.h items.h itemname.h itemprop.h misc.h monplace.h
item_use.o: monstuff.h mstuff2.h mon-util.h ouch.h player.h randart.h
item_use.o: religion.h skills.h skills2.h spells1.h spells2.h spells3.h
item_use.o: spl-book.h spl-cast.h stuff.h transfor.h view.h
itemname.o: AppHdr.h libutil.h itemname.h externs.h
itemname.o: defines.h enum.h FixAry.h FixVec.h
itemname.o: debug.h Kills.h
itemname.o: message.h
itemname.o: globals.h invent.h menu.h items.h
itemname.o: itemprop.h macro.h mon-util.h player.h randart.h skills2.h
itemname.o: stuff.h transfor.h view.h
itemprop.o: AppHdr.h libutil.h itemname.h externs.h
itemprop.o: defines.h enum.h FixAry.h FixVec.h
itemprop.o: debug.h Kills.h
itemprop.o: message.h
itemprop.o: globals.h items.h itemprop.h macro.h
itemprop.o: mon-util.h player.h randart.h skills2.h stuff.h transfor.h view.h
items.o: AppHdr.h libutil.h items.h externs.h
items.o: defines.h enum.h FixAry.h FixVec.h
items.o: debug.h Kills.h
items.o: message.h globals.h
items.o: beam.h cloud.h delay.h effects.h invent.h menu.h it_use2.h
items.o: item_use.h itemname.h itemprop.h misc.h monplace.h monstuff.h
items.o: mstuff2.h mon-util.h mutation.h player.h randart.h religion.h
items.o: shopping.h skills.h skills2.h spl-cast.h stuff.h view.h
lev-pand.o: AppHdr.h libutil.h lev-pand.h globals.h defines.h externs.h
lev-pand.o: enum.h
lev-pand.o: FixAry.h FixVec.h debug.h Kills.h
lev-pand.o: message.h monplace.h mon-pick.h
lev-pand.o: stuff.h
libemx.o: libemx.h
liblinux.o: AppHdr.h libutil.h
liblinux.o: liblinux.h defines.h enum.h globals.h
liblinux.o: externs.h FixAry.h
liblinux.o: FixVec.h debug.h Kills.h message.h
libmac.o: AppHdr.h libutil.h libmac.h
libutil.o: AppHdr.h libutil.h defines.h
libw32c.o: AppHdr.h libutil.h version.h defines.h
libw32c.o: view.h externs.h
libw32c.o: enum.h FixAry.h FixVec.h debug.h Kills.h message.h
macro.o: AppHdr.h libutil.h macro.h
macro.o: globals.h defines.h externs.h
macro.o: enum.h FixAry.h
macro.o: FixVec.h debug.h Kills.h message.h initfile.h
maps.o: AppHdr.h libutil.h maps.h FixVec.h debug.h
maps.o: enum.h globals.h
maps.o: defines.h externs.h
maps.o: FixAry.h Kills.h
maps.o: message.h monplace.h stuff.h
menu.o: AppHdr.h libutil.h globals.h defines.h externs.h
menu.o: enum.h FixAry.h FixVec.h debug.h
menu.o: Kills.h
menu.o: message.h menu.h view.h
message.o: AppHdr.h libutil.h message.h externs.h
message.o: defines.h enum.h FixAry.h FixVec.h
message.o: debug.h Kills.h
message.o: religion.h globals.h macro.h delay.h
message.o: player.h misc.h stuff.h view.h
misc.o: AppHdr.h libutil.h misc.h externs.h
misc.o: defines.h enum.h FixAry.h FixVec.h
misc.o: debug.h Kills.h
misc.o: message.h
misc.o: globals.h cloud.h delay.h dungeon.h effects.h
misc.o: fight.h files.h food.h it_use2.h items.h itemprop.h lev-pand.h
misc.o: macro.h monplace.h mon-util.h monstuff.h ouch.h player.h shopping.h
misc.o: skills.h skills2.h spells3.h spl-cast.h stuff.h transfor.h travel.h
misc.o: view.h
mon-pick.o: AppHdr.h libutil.h mon-pick.h globals.h defines.h externs.h
mon-pick.o: enum.h
mon-pick.o: FixAry.h FixVec.h debug.h Kills.h
mon-pick.o: message.h
mon-util.o: AppHdr.h libutil.h mon-util.h externs.h
mon-util.o: defines.h enum.h FixAry.h FixVec.h
mon-util.o: debug.h Kills.h
mon-util.o: message.h monstuff.h
mon-util.o: globals.h beam.h fight.h itemname.h
mon-util.o: itemprop.h misc.h mstuff2.h player.h randart.h skills2.h stuff.h
mon-util.o: view.h mon-data.h mon-spll.h
monplace.o: AppHdr.h libutil.h monplace.h defines.h enum.h FixVec.h debug.h
monplace.o: beam.h externs.h
monplace.o: FixAry.h Kills.h message.h globals.h dungeon.h monstuff.h
monplace.o: mon-pick.h mon-util.h misc.h player.h stuff.h spells4.h view.h
monspeak.o: AppHdr.h libutil.h monspeak.h externs.h
monspeak.o: defines.h enum.h FixAry.h FixVec.h
monspeak.o: debug.h Kills.h
monspeak.o: message.h globals.h
monspeak.o: beam.h fight.h insult.h itemname.h misc.h monplace.h monstuff.h
monspeak.o: mon-util.h mstuff2.h player.h spells2.h spells4.h stuff.h view.h
monstuff.o: AppHdr.h libutil.h monstuff.h externs.h
monstuff.o: defines.h enum.h FixAry.h FixVec.h
monstuff.o: debug.h Kills.h
monstuff.o: message.h globals.h
monstuff.o: beam.h cloud.h dungeon.h fight.h itemname.h itemprop.h items.h
monstuff.o: misc.h monplace.h monspeak.h mon-util.h mstuff2.h player.h
monstuff.o: randart.h religion.h spl-cast.h spells1.h direct.h spells2.h
monstuff.o: spells4.h stuff.h view.h
mstuff2.o: AppHdr.h libutil.h mstuff2.h externs.h
mstuff2.o: defines.h enum.h FixAry.h FixVec.h
mstuff2.o: debug.h Kills.h
mstuff2.o: message.h globals.h beam.h effects.h fight.h
mstuff2.o: itemname.h itemprop.h items.h misc.h monplace.h monstuff.h
mstuff2.o: mon-util.h player.h spells2.h spells4.h spl-cast.h stuff.h view.h
mutation.o: AppHdr.h libutil.h mutation.h enum.h
mutation.o: globals.h defines.h externs.h
mutation.o: FixAry.h FixVec.h
mutation.o: debug.h Kills.h message.h effects.h macro.h ouch.h player.h
mutation.o: skills2.h stuff.h transfor.h view.h
newgame.o: AppHdr.h libutil.h newgame.h
newgame.o: globals.h defines.h externs.h enum.h
newgame.o: FixAry.h FixVec.h debug.h Kills.h message.h abl-show.h dungeon.h
newgame.o: files.h fight.h itemname.h itemprop.h items.h macro.h player.h
newgame.o: randart.h skills.h skills2.h spl-util.h direct.h stuff.h version.h
ouch.o: AppHdr.h libutil.h ouch.h externs.h
ouch.o: defines.h enum.h FixAry.h FixVec.h
ouch.o: debug.h Kills.h
ouch.o: message.h
ouch.o: globals.h chardump.h
ouch.o: delay.h files.h hiscores.h invent.h menu.h itemname.h itemprop.h
ouch.o: items.h macro.h mon-util.h player.h randart.h religion.h shopping.h
ouch.o: skills2.h spells4.h stuff.h view.h
output.o: AppHdr.h libutil.h output.h
output.o: globals.h defines.h externs.h
output.o: enum.h FixAry.h FixVec.h debug.h
output.o: Kills.h
output.o: message.h fight.h itemname.h itemprop.h ouch.h player.h religion.h
output.o: skills2.h stuff.h
overmap.o: AppHdr.h libutil.h overmap.h
overmap.o: globals.h defines.h externs.h
overmap.o: enum.h FixAry.h FixVec.h debug.h Kills.h
overmap.o: message.h files.h misc.h religion.h stuff.h view.h
player.o: AppHdr.h libutil.h player.h externs.h
player.o: defines.h enum.h FixAry.h FixVec.h
player.o: debug.h Kills.h
player.o: message.h
player.o: globals.h cloud.h delay.h effects.h
player.o: fight.h itemname.h items.h itemprop.h macro.h misc.h mon-util.h
player.o: mutation.h output.h ouch.h randart.h religion.h skills.h skills2.h
player.o: spl-util.h direct.h spells4.h stuff.h transfor.h view.h
randart.o: AppHdr.h libutil.h randart.h enum.h externs.h
randart.o: defines.h FixAry.h FixVec.h debug.h
randart.o: Kills.h
randart.o: message.h dungeon.h globals.h itemname.h
randart.o: itemprop.h shopping.h stuff.h unrand.h
religion.o: AppHdr.h libutil.h religion.h enum.h
religion.o: globals.h defines.h externs.h
religion.o: FixAry.h FixVec.h
religion.o: debug.h Kills.h message.h abl-show.h beam.h decks.h describe.h
religion.o: dungeon.h effects.h food.h item_use.h it_use2.h itemname.h
religion.o: itemprop.h items.h misc.h monplace.h mon-util.h mutation.h
religion.o: newgame.h ouch.h player.h randart.h shopping.h skills2.h
religion.o: spells1.h direct.h spells2.h spells3.h spl-cast.h stuff.h view.h
shopping.o: AppHdr.h libutil.h chardump.h shopping.h externs.h
shopping.o: defines.h enum.h FixAry.h FixVec.h debug.h Kills.h
shopping.o: message.h
shopping.o: globals.h describe.h invent.h menu.h
shopping.o: items.h itemname.h itemprop.h macro.h player.h randart.h
shopping.o: spl-book.h spl-util.h direct.h stash.h stuff.h
skills.o: AppHdr.h libutil.h skills.h
skills.o: globals.h defines.h externs.h
skills.o: enum.h FixAry.h
skills.o: FixVec.h debug.h Kills.h message.h macro.h player.h skills2.h
skills.o: stuff.h
skills2.o: AppHdr.h libutil.h skills2.h enum.h
skills2.o: globals.h defines.h externs.h
skills2.o: FixAry.h FixVec.h
skills2.o: debug.h Kills.h message.h fight.h itemprop.h player.h randart.h
skills2.o: religion.h skills.h stuff.h transfor.h view.h
spells1.o: AppHdr.h libutil.h spells1.h externs.h
spells1.o: defines.h enum.h FixAry.h FixVec.h
spells1.o: debug.h Kills.h
spells1.o: message.h direct.h
spells1.o: globals.h abyss.h beam.h cloud.h invent.h menu.h it_use2.h
spells1.o: itemname.h itemprop.h misc.h monplace.h monstuff.h mon-util.h
spells1.o: ouch.h player.h skills2.h spells2.h spells3.h spells4.h spl-util.h
spells1.o: stuff.h view.h
spells2.o: AppHdr.h libutil.h spells2.h enum.h
spells2.o: globals.h defines.h externs.h
spells2.o: FixAry.h FixVec.h
spells2.o: debug.h Kills.h message.h beam.h cloud.h direct.h effects.h
spells2.o: itemname.h itemprop.h items.h misc.h monplace.h monstuff.h
spells2.o: mon-util.h ouch.h player.h randart.h spells4.h spl-cast.h
spells2.o: spl-util.h stuff.h view.h
spells3.o: AppHdr.h libutil.h spells3.h
spells3.o: globals.h
spells3.o: defines.h externs.h
spells3.o: enum.h FixAry.h FixVec.h debug.h Kills.h message.h abyss.h beam.h
spells3.o: cloud.h direct.h delay.h itemname.h itemprop.h items.h it_use2.h
spells3.o: misc.h monplace.h mon-pick.h monstuff.h mon-util.h ouch.h player.h
spells3.o: randart.h spells1.h spl-cast.h spl-util.h stuff.h view.h
spells4.o: AppHdr.h libutil.h
spells4.o: globals.h defines.h externs.h
spells4.o: enum.h FixAry.h FixVec.h debug.h Kills.h
spells4.o: message.h abyss.h beam.h cloud.h delay.h describe.h direct.h
spells4.o: dungeon.h effects.h it_use2.h itemname.h itemprop.h items.h
spells4.o: invent.h menu.h misc.h monplace.h monstuff.h mon-util.h mstuff2.h
spells4.o: ouch.h player.h randart.h religion.h skills.h spells1.h spells3.h
spells4.o: spells4.h spl-cast.h spl-util.h stuff.h transfor.h view.h
spl-book.o: AppHdr.h libutil.h spl-book.h externs.h
spl-book.o: defines.h enum.h FixAry.h FixVec.h
spl-book.o: debug.h Kills.h
spl-book.o: message.h
spl-book.o: globals.h delay.h food.h invent.h
spl-book.o: menu.h itemname.h itemprop.h items.h it_use3.h player.h
spl-book.o: religion.h spl-cast.h spl-util.h direct.h stuff.h
spl-cast.o: AppHdr.h libutil.h spl-cast.h
spl-cast.o: globals.h defines.h externs.h
spl-cast.o: enum.h FixAry.h
spl-cast.o: FixVec.h debug.h Kills.h message.h beam.h cloud.h effects.h
spl-cast.o: fight.h food.h it_use2.h itemname.h itemprop.h macro.h monplace.h
spl-cast.o: monstuff.h mutation.h ouch.h player.h religion.h skills.h
spl-cast.o: spells1.h direct.h spells2.h spells3.h spells4.h spl-book.h
spl-cast.o: spl-util.h stuff.h transfor.h view.h
spl-util.o: AppHdr.h libutil.h spl-util.h enum.h direct.h externs.h
spl-util.o: defines.h FixAry.h FixVec.h debug.h Kills.h
spl-util.o: message.h
spl-util.o: globals.h stuff.h itemname.h macro.h misc.h
spl-util.o: monstuff.h player.h spl-book.h view.h spl-data.h
stash.o: AppHdr.h libutil.h chardump.h describe.h externs.h
stash.o: defines.h enum.h FixAry.h FixVec.h
stash.o: debug.h Kills.h
stash.o: message.h itemname.h files.h stash.h globals.h tags.h travel.h
stuff.o: AppHdr.h libutil.h stuff.h enum.h defines.h externs.h
stuff.o: FixAry.h FixVec.h debug.h Kills.h
stuff.o: message.h
stuff.o: mt19937ar.h globals.h macro.h delay.h misc.h monstuff.h mon-util.h
stuff.o: player.h output.h skills2.h view.h
tags.o: AppHdr.h libutil.h abl-show.h enum.h
tags.o: dungeon.h FixVec.h debug.h externs.h
tags.o: defines.h FixAry.h Kills.h message.h
tags.o: globals.h files.h itemname.h itemprop.h monstuff.h mon-util.h
tags.o: randart.h skills.h skills2.h stuff.h tags.h view.h
transfor.o: AppHdr.h libutil.h transfor.h FixVec.h debug.h enum.h
transfor.o: globals.h defines.h externs.h
transfor.o: FixAry.h Kills.h
transfor.o: message.h effects.h item_use.h it_use2.h itemname.h itemprop.h
transfor.o: items.h misc.h player.h skills2.h stuff.h
travel.o: AppHdr.h libutil.h files.h FixAry.h FixVec.h debug.h
travel.o: misc.h externs.h
travel.o: defines.h enum.h Kills.h message.h mon-util.h player.h stash.h
travel.o: globals.h stuff.h travel.h view.h
view.o: AppHdr.h libutil.h view.h externs.h
view.o: defines.h enum.h FixAry.h FixVec.h
view.o: debug.h Kills.h
view.o: message.h
view.o: globals.h cloud.h
view.o: delay.h insult.h itemprop.h macro.h misc.h monstuff.h mon-util.h
view.o: overmap.h player.h skills2.h stuff.h spells4.h travel.h
AppHdr.o: libutil.h
FixAry.o: FixVec.h debug.h
FixVec.o: debug.h
abl-show.o: enum.h
beam.o: externs.h
beam.o: defines.h
beam.o: enum.h FixAry.h FixVec.h debug.h Kills.h
beam.o: message.h
cloud.o: externs.h
cloud.o: defines.h
cloud.o: enum.h FixAry.h FixVec.h debug.h Kills.h
cloud.o: message.h
describe.o: externs.h
describe.o: defines.h enum.h FixAry.h FixVec.h debug.h Kills.h
describe.o: message.h
direct.o: externs.h
direct.o: defines.h enum.h FixAry.h FixVec.h debug.h Kills.h
direct.o: message.h
dungeon.o: FixVec.h debug.h externs.h
dungeon.o: defines.h enum.h FixAry.h Kills.h
dungeon.o: message.h
effects.o: externs.h
effects.o: defines.h enum.h FixAry.h FixVec.h debug.h Kills.h
effects.o: message.h
externs.o: defines.h enum.h FixAry.h FixVec.h debug.h Kills.h
externs.o: message.h externs.h
fight.o: externs.h
fight.o: defines.h
fight.o: enum.h FixAry.h FixVec.h debug.h Kills.h
fight.o: message.h
files.o: FixAry.h FixVec.h debug.h
globals.o: defines.h externs.h
globals.o: enum.h
globals.o: FixAry.h FixVec.h debug.h Kills.h
globals.o: message.h
insult.o: externs.h
insult.o: defines.h enum.h FixAry.h FixVec.h debug.h Kills.h
insult.o: message.h
invent.o: menu.h AppHdr.h libutil.h defines.h
it_use2.o: externs.h
it_use2.o: defines.h enum.h FixAry.h FixVec.h debug.h Kills.h
it_use2.o: message.h
itemname.o: externs.h
itemname.o: defines.h enum.h FixAry.h FixVec.h debug.h Kills.h
itemname.o: message.h
itemprop.o: externs.h
itemprop.o: defines.h enum.h FixAry.h FixVec.h debug.h Kills.h
itemprop.o: message.h
items.o: externs.h
items.o: defines.h
items.o: enum.h FixAry.h FixVec.h debug.h Kills.h
items.o: message.h
maps.o: FixVec.h debug.h
menu.o: AppHdr.h libutil.h defines.h
message.o: externs.h
message.o: defines.h enum.h FixAry.h FixVec.h debug.h Kills.h
message.o: message.h
misc.o: externs.h
misc.o: defines.h
misc.o: enum.h FixAry.h FixVec.h debug.h Kills.h
misc.o: message.h
mon-util.o: externs.h
mon-util.o: defines.h enum.h FixAry.h FixVec.h debug.h Kills.h
mon-util.o: message.h
monplace.o: defines.h enum.h FixVec.h debug.h
monspeak.o: externs.h
monspeak.o: defines.h enum.h FixAry.h FixVec.h debug.h Kills.h
monspeak.o: message.h
monstuff.o: externs.h
monstuff.o: defines.h enum.h FixAry.h FixVec.h debug.h Kills.h
monstuff.o: message.h
mstuff2.o: externs.h
mstuff2.o: defines.h enum.h FixAry.h FixVec.h debug.h Kills.h
mstuff2.o: message.h
mutation.o: enum.h
ouch.o: externs.h
ouch.o: defines.h
ouch.o: enum.h FixAry.h FixVec.h debug.h Kills.h
ouch.o: message.h
player.o: externs.h
player.o: defines.h enum.h FixAry.h FixVec.h debug.h Kills.h
player.o: message.h
randart.o: enum.h externs.h
randart.o: defines.h FixAry.h FixVec.h debug.h Kills.h
randart.o: message.h
religion.o: enum.h
shopping.o: externs.h
shopping.o: defines.h enum.h FixAry.h FixVec.h debug.h Kills.h
shopping.o: message.h
skills2.o: enum.h
spells1.o: externs.h
spells1.o: defines.h enum.h FixAry.h FixVec.h debug.h Kills.h
spells1.o: message.h direct.h
spells2.o: enum.h
spells4.o: externs.h
spells4.o: defines.h enum.h FixAry.h FixVec.h debug.h Kills.h
spells4.o: message.h
spl-book.o: externs.h
spl-book.o: defines.h enum.h FixAry.h FixVec.h debug.h Kills.h
spl-book.o: message.h
spl-util.o: enum.h direct.h externs.h
spl-util.o: defines.h FixAry.h FixVec.h debug.h
spl-util.o: Kills.h
spl-util.o: message.h
stash.o: AppHdr.h libutil.h globals.h defines.h externs.h
stash.o: enum.h FixAry.h FixVec.h debug.h
stash.o: Kills.h
stash.o: message.h
stuff.o: enum.h defines.h externs.h
stuff.o: FixAry.h FixVec.h debug.h Kills.h
stuff.o: message.h
tags.o: externs.h defines.h
tags.o: enum.h FixAry.h FixVec.h debug.h Kills.h message.h
transfor.o: FixVec.h debug.h enum.h
travel.o: externs.h
travel.o: defines.h enum.h FixAry.h FixVec.h debug.h Kills.h
travel.o: message.h
unrand.o: defines.h
view.o: externs.h
view.o: defines.h
view.o: enum.h FixAry.h FixVec.h debug.h Kills.h
view.o: message.h

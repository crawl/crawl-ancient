# Make file for Dungeon Crawl (linux port only for now)

C_COMP = g++
C_ARGS = -Wall
L_ARGS = -static
MCHMOD = 711
INSTALLDIR = /usr/games
LIB = -lncurses


# Include for Linux
INCLUDE = -I/usr/include/ncurses

all:		crawl

install:	crawl
		cp crawl ${INSTALLDIR}
		chmod ${MCHMOD} ${INSTALLDIR}/crawl

clean:
		rm -f *.o

distclean:
		rm -f *.o morgue.txt scores crawl


crawl:		acr.o bang.o beam.o chardump.o command.o debug.o decks.o describe.o direct.o dungeon.o effects.o fight.o files.o food.o invent.o it_use2.o it_use3.o item_use.o itemname.o items.o levels.o linuxlib.o maps.o message.o misc.o monplace.o mons_lev.o monstuff.o mstat2.o mstruct.o mstuff2.o newgame.o ouch.o output.o player.o priest.o shopping.o skills.o skills2.o spell.o spells.o spells0.o spells1.o spells2.o spells3.o stuff.o view.o
		${C_COMP} ${L_ARGS} -o crawl acr.o bang.o beam.o chardump.o command.o debug.o decks.o describe.o direct.o dungeon.o effects.o fight.o files.o food.o invent.o it_use2.o it_use3.o item_use.o itemname.o items.o levels.o linuxlib.o maps.o message.o misc.o monplace.o mons_lev.o monstuff.o mstat2.o mstruct.o mstuff2.o newgame.o ouch.o output.o player.o priest.o shopping.o skills.o skills2.o spell.o spells.o spells0.o spells1.o spells2.o spells3.o stuff.o view.o ${LIB}
		strip crawl
		chmod ${MCHMOD} crawl


acr.o:		acr.cc
		${C_COMP} ${C_ARGS} -c acr.cc ${INCLUDE}

bang.o:		bang.cc
		${C_COMP} ${C_ARGS} -c bang.cc ${INCLUDE}

beam.o:		beam.cc
		${C_COMP} ${C_ARGS} -c beam.cc ${INCLUDE}

chardump.o:	chardump.cc
		${C_COMP} ${C_ARGS} -c chardump.cc ${INCLUDE}

command.o:	command.cc
		${C_COMP} ${C_ARGS} -c command.cc ${INCLUDE}

debug.o:	debug.cc
		${C_COMP} ${C_ARGS} -c debug.cc ${INCLUDE}

decks.o:	decks.cc
		${C_COMP} ${C_ARGS} -c decks.cc ${INCLUDE}

describe.o:	describe.cc
		${C_COMP} ${C_ARGS} -c describe.cc ${INCLUDE}

direct.o:	direct.cc
		${C_COMP} ${C_ARGS} -c direct.cc ${INCLUDE}

dungeon.o:	dungeon.cc
		${C_COMP} ${C_ARGS} -c dungeon.cc ${INCLUDE}

effects.o:	effects.cc
		${C_COMP} ${C_ARGS} -c effects.cc ${INCLUDE}

fight.o:	fight.cc
		${C_COMP} ${C_ARGS} -c fight.cc ${INCLUDE}

files.o:	files.cc
		${C_COMP} ${C_ARGS} -c files.cc ${INCLUDE}

food.o:		food.cc
		${C_COMP} ${C_ARGS} -c food.cc ${INCLUDE}

invent.o:	invent.cc
		${C_COMP} ${C_ARGS} -c invent.cc ${INCLUDE}

it_use2.o:	it_use2.cc
		${C_COMP} ${C_ARGS} -c it_use2.cc ${INCLUDE}

it_use3.o:	it_use3.cc
		${C_COMP} ${C_ARGS} -c it_use3.cc ${INCLUDE}

item_use.o:	item_use.cc
		${C_COMP} ${C_ARGS} -c item_use.cc ${INCLUDE}

itemname.o:	itemname.cc
		${C_COMP} ${C_ARGS} -c itemname.cc ${INCLUDE}

items.o:	items.cc
		${C_COMP} ${C_ARGS} -c items.cc ${INCLUDE}

levels.o:	levels.cc
		${C_COMP} ${C_ARGS} -c levels.cc ${INCLUDE}

linuxlib.o:	linuxlib.cc
		${C_COMP} ${C_ARGS} -c linuxlib.cc ${INCLUDE}

maps.o:		maps.cc
		${C_COMP} ${C_ARGS} -c maps.cc ${INCLUDE}

message.o:	message.cc
		${C_COMP} ${C_ARGS} -c message.cc ${INCLUDE}

misc.o:		misc.cc
		${C_COMP} ${C_ARGS} -c misc.cc ${INCLUDE}

monplace.o:	monplace.cc
		${C_COMP} ${C_ARGS} -c monplace.cc ${INCLUDE}

mons_lev.o:	mons_lev.cc
		${C_COMP} ${C_ARGS} -c mons_lev.cc ${INCLUDE}

monstuff.o:	monstuff.cc
		${C_COMP} ${C_ARGS} -c monstuff.cc ${INCLUDE}

mstat2.o:	mstat2.cc
		${C_COMP} ${C_ARGS} -c mstat2.cc ${INCLUDE}

mstruct.o:	mstruct.cc
		${C_COMP} ${C_ARGS} -c mstruct.cc ${INCLUDE}

mstuff2.o:	mstuff2.cc
		${C_COMP} ${C_ARGS} -c mstuff2.cc ${INCLUDE}

newgame.o:	newgame.cc
		${C_COMP} ${C_ARGS} -c newgame.cc ${INCLUDE}

ouch.o:		ouch.cc
		${C_COMP} ${C_ARGS} -c ouch.cc ${INCLUDE}

output.o:	output.cc
		${C_COMP} ${C_ARGS} -c output.cc ${INCLUDE}

player.o:	player.cc
		${C_COMP} ${C_ARGS} -c player.cc ${INCLUDE}

priest.o:	priest.cc
		${C_COMP} ${C_ARGS} -c priest.cc ${INCLUDE}

shopping.o:	shopping.cc
		${C_COMP} ${C_ARGS} -c shopping.cc ${INCLUDE}

skills.o:	skills.cc
		${C_COMP} ${C_ARGS} -c skills.cc ${INCLUDE}

skills2.o:	skills2.cc
		${C_COMP} ${C_ARGS} -c skills2.cc ${INCLUDE}

spell.o:	spell.cc
		${C_COMP} ${C_ARGS} -c spell.cc ${INCLUDE}

spells.o:	spells.cc
		${C_COMP} ${C_ARGS} -c spells.cc ${INCLUDE}

spells0.o:	spells0.cc
		${C_COMP} ${C_ARGS} -c spells0.cc ${INCLUDE}

spells1.o:	spells1.cc
		${C_COMP} ${C_ARGS} -c spells1.cc ${INCLUDE}

spells2.o:	spells2.cc
		${C_COMP} ${C_ARGS} -c spells2.cc ${INCLUDE}

spells3.o:	spells3.cc
		${C_COMP} ${C_ARGS} -c spells3.cc ${INCLUDE}

stuff.o:	stuff.cc
		${C_COMP} ${C_ARGS} -c stuff.cc ${INCLUDE}

view.o:		view.cc
		${C_COMP} ${C_ARGS} -c view.cc ${INCLUDE}

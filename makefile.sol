# Make file for Dungeon Crawl (linux port only for now)

C_COMP = g++
C_ARGS = -Wall -O -DSOLARIS
L_ARGS = -s
MCHMOD = 2755
GROUP = games
INSTALLDIR = /opt/crawl/bin
LIB = -lcurses

include makefile.obj

#INCLUDE = -I/opt/include/

all:            crawl

install:        crawl
		rm -f ${INSTALLDIR}/crawl.old
		mv -f ${INSTALLDIR}/crawl ${INSTALLDIR}/crawl.old
		cp crawl ${INSTALLDIR}/crawl
		chgrp ${GROUP} ${INSTALLDIR}/crawl
		chmod ${MCHMOD} ${INSTALLDIR}/crawl

clean:
		rm -f *.o

distclean:
		rm -f *.o bones.* morgue.txt scores crawl

crawl:	${OBJECTS}
		${C_COMP} ${L_ARGS} -o $@ ${OBJECTS} ${LIB}
		chmod ${MCHMOD} $@

.cc.o:
		${C_COMP} ${C_ARGS} -c $< ${INCLUDE}

# Make file for Dungeon Crawl (linux)

APPNAME = crawl

# this file contains a list of the libraries.
# it will make a variable called OBJECTS that contains all the libraries
include make.obj

CXX = g++
OS_TYPE = LINUX
CFLAGS = -Wall -D$(OS_TYPE)
LDFLAGS = -static -L/usr/lib
MCHMOD = 711
INSTALLDIR = /usr/games
LIB = -lncurses

# Include for Linux
INCLUDES = -I/usr/include/ncurses

all:            $(APPNAME)

install:        $(APPNAME)
		cp $(APPNAME) ${INSTALLDIR}
		chmod ${MCHMOD} ${INSTALLDIR}/$(APPNAME)

clean:
		rm -f *.o

distclean:
		rm -f *.o bones.* morgue.txt scores $(APPNAME) *sav


$(APPNAME):	$(OBJECTS)
		${CXX} ${LDFLAGS} $(INCLUDES) $(CFLAGS) $(OBJECTS) -o $(APPNAME) $(LIB)
		strip $(APPNAME)
		chmod ${MCHMOD} $(APPNAME)

debug:		$(OBJECTS)
		${CXX} -g ${LDFLAGS} $(INCLUDES) $(CFLAGS) $(OBJECTS) -o $(APPNAME) $(LIB)

profile:	$(OBJECTS)
		${CXX} -g -p ${LDFLAGS} $(INCLUDES) $(CFLAGS) $(OBJECTS) -o $(APPNAME) $(LIB)

.cc.o:
		${CXX} -g ${CFLAGS} -c $< ${INCLUDE}

.h.cc:
		touch $@

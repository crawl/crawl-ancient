# Make file for Dungeon Crawl (linux)

APPNAME = crawl

# this file contains a list of the libraries.
# it will make a variable called OBJECTS that contains all the libraries
include makefile.obj

OBJECTS += liblinux.o

CXX = g++
DELETE = rm -f
COPY = cp
OS_TYPE = SOLARIS
CFLAGS = -Wall -D$(OS_TYPE) $(EXTRA_FLAGS)
LDFLAGS = -static
MCHMOD = 2755
INSTALLDIR = /opt/crawl/bin
LIB = -lcurses

all:            $(APPNAME)

install:        $(APPNAME)
		$(COPY) ${INSTALLDIR}/${APPNAME} ${INSTALLDIR}/${APPNAME}.old
		$(COPY) $(APPNAME) ${INSTALLDIR}
		chmod ${MCHMOD} ${INSTALLDIR}/$(APPNAME)

clean:
		$(DELETE) *.o

distclean:
		$(DELETE) *.o
		$(DELETE) bones.*
		$(DELETE) morgue.txt
		$(DELETE) scores
		$(DELETE) $(APPNAME)
		$(DELETE) *.sav
		$(DELETE) core
		$(DELETE) *.0*
		$(DELETE) *.lab


$(APPNAME):	$(OBJECTS)
		${CXX} ${LDFLAGS} $(INCLUDES) $(CFLAGS) $(OBJECTS) -o $(APPNAME) $(LIB)
		strip $(APPNAME)
		chmod ${MCHMOD} $(APPNAME)

debug:		$(OBJECTS)
		${CXX} ${LDFLAGS} $(INCLUDES) $(CFLAGS) $(OBJECTS) -o $(APPNAME) $(LIB)

profile:	$(OBJECTS)
		${CXX} -g -p ${LDFLAGS} $(INCLUDES) $(CFLAGS) $(OBJECTS) -o $(APPNAME) $(LIB)

.cc.o:
		${CXX} ${CFLAGS} -c $< ${INCLUDE}

.h.cc:
		touch $@

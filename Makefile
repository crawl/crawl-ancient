#Makefile chooser.  Choose one:

MAKEFILE = makefile.lnx
#MAKEFILE = makefile.dos
#MAKEFILE = makefile.emx
#MAKEFILE = makefile.sol

all:
	make -f $(MAKEFILE) EXTRA_FLAGS=-O3
noopt:
	make -f $(MAKEFILE)
clean:
	make -f $(MAKEFILE) clean
distclean:
	make -f $(MAKEFILE) distclean
debug:
	make -f $(MAKEFILE) debug EXTRA_FLAGS=-g
wizard:
	make -f $(MAKEFILE) debug EXTRA_FLAGS='-g -DWIZARD'

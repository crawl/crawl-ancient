#Makefile chooser.  Choose one:

#MAKEFILE = makefile.lnx
#MAKEFILE = makefile.sgi
#MAKEFILE = makefile.dos
#MAKEFILE = makefile.emx
MAKEFILE = makefile.sol

all:
	make -f $(MAKEFILE) EXTRA_FLAGS=-O3
noopt:
	make -f $(MAKEFILE)
install:
	make -f $(MAKEFILE) install
clean:
	make -f $(MAKEFILE) clean
distclean:
	make -f $(MAKEFILE) distclean
debug:
	make -f $(MAKEFILE) debug EXTRA_FLAGS=-g
wizard:
	make -f $(MAKEFILE) debug EXTRA_FLAGS='-g -DWIZARD'
# DO NOT DELETE THIS LINE -- make depend depends on it.

#Makefile chooser.  Choose one:

MAKEFILE = makefile.lnx
#MAKEFILE = makefile.dos
#MAKEFILE = makefile.emx
#MAKEFILE = makefile.sol

all:
	make -f $(MAKEFILE) EXTRA_FLAGS=-O3
clean:
	make -f $(MAKEFILE) clean
debug:
	make -f $(MAKEFILE) debug EXTRA_FLAGS=-g

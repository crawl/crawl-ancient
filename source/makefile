#Makefile chooser.  Choose one:

MAKEFILE = makefile.lnx
#MAKEFILE = makefile.sgi
#MAKEFILE = makefile.dos
#MAKEFILE = makefile.emx
#MAKEFILE = makefile.sol

#jmf: number of concurrent jobs -- good value is (#_of_CPUs * 2) + 1
#     cuts build time a lot on multi-cpu machines
#OTHER=-j3

all:
	make $(OTHER) -f $(MAKEFILE) EXTRA_FLAGS='-O2 -fno-strength-reduce'
noopt:
	make $(OTHER) -f $(MAKEFILE)
install:
	make $(OTHER) -f $(MAKEFILE) install
clean:
	make $(OTHER) -f $(MAKEFILE) clean
distclean:
	make $(OTHER) -f $(MAKEFILE) distclean
debug:
	make $(OTHER) -f $(MAKEFILE) debug EXTRA_FLAGS='-g -DDEBUG'
wizard:
	make $(OTHER) -f $(MAKEFILE) debug EXTRA_FLAGS='-g -DWIZARD -DDEBUG'
# DO NOT DELETE THIS LINE -- make depend depends on it.

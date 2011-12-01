#
include ../include/$(GEM_ARCH)
IDIR = $(CURDIR)/../include
ifdef GEM_BLOC
LDIR = $(GEM_BLOC)/lib
TDIR = $(GEM_BLOC)/test
else
LDIR = ../../$(GEM_ARCH)/lib
TDIR = ../../$(GEM_ARCH)/test
endif

default:	$(TDIR)/testc $(TDIR)/testf $(TDIR)/parsec $(TDIR)/parsef

$(TDIR)/testc:		$(TDIR)/testc.o $(LDIR)/$(SHLIB)
	$(CC) -o $(TDIR)/testc $(TDIR)/testc.o -L$(LDIR) -legads -lm

$(TDIR)/testc.o:	testc.c $(IDIR)/egads.h $(IDIR)/egadsTypes.h \
		$(IDIR)/egadsErrors.h
	$(CCOMP) -c $(COPTS) $(DEFINE) $(INCS) testc.c -o $(TDIR)/testc.o

$(TDIR)/testf:		$(TDIR)/testf.o $(LDIR)/libfgads.a $(LDIR)/$(SHLIB)
	$(FCOMP) -o $(TDIR)/testf $(TDIR)/testf.o -L$(LDIR) -lfgads -legads \
		-lstdc++

$(TDIR)/testf.o:	testf.f
	$(FCOMP) -c $(FOPTS) -I$(IDIR) testf.f -o $(TDIR)/testf.o

$(TDIR)/parsec:		$(TDIR)/parsec.o $(LDIR)/$(SHLIB)
	$(CC) -o $(TDIR)/parsec $(TDIR)/parsec.o -L$(LDIR) -legads -lm

$(TDIR)/parsec.o:	parsec.c $(IDIR)/egads.h $(IDIR)/egadsTypes.h \
		$(IDIR)/egadsErrors.h
	$(CCOMP) -c $(COPTS) $(DEFINE) $(INCS) parsec.c -o $(TDIR)/parsec.o

$(TDIR)/parsef:		$(TDIR)/parsef.o $(LDIR)/libfgads.a $(LDIR)/$(SHLIB)
	$(FCOMP) -o $(TDIR)/parsef $(TDIR)/parsef.o -L$(LDIR) -lfgads -legads \
		-lstdc++

$(TDIR)/parsef.o:	parsef.f
	$(FCOMP) -c $(FOPTS) -I$(IDIR) parsef.f -o $(TDIR)/parsef.o

clean:
	(cd $(TDIR); \
	rm testc.o testc testf.o testf parsec.o parsec parsef.o parsef )

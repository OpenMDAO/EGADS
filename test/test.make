#
include ../include/$(GEM_ARCH)
LDIR = ../../$(GEM_ARCH)/lib
TDIR = ../../$(GEM_ARCH)/test

default:	$(TDIR)/testc $(TDIR)/testf $(TDIR)/parsec $(TDIR)/parsef

$(TDIR)/testc:		$(TDIR)/testc.o $(LDIR)/$(SHLIB)
	$(CC) -o $(TDIR)/testc $(TDIR)/testc.o -L$(LDIR) -legads -lm

$(TDIR)/testc.o:	testc.c ../include/egads.h ../include/egadsTypes.h \
		../include/egadsErrors.h
	$(CCOMP) -c $(COPTS) $(DEFINE) $(INCS) testc.c -o $(TDIR)/testc.o

$(TDIR)/testf:		$(TDIR)/testf.o $(LDIR)/libfgads.a $(LDIR)/$(SHLIB)
	$(FCOMP) -o $(TDIR)/testf $(TDIR)/testf.o -L$(LDIR) -lfgads -legads \
		-lstdc++

$(TDIR)/testf.o:	testf.f
	$(FCOMP) -c $(FOPTS) -I../include testf.f -o $(TDIR)/testf.o

$(TDIR)/parsec:		$(TDIR)/parsec.o $(LDIR)/$(SHLIB)
	$(CC) -o $(TDIR)/parsec $(TDIR)/parsec.o -L$(LDIR) -legads -lm

$(TDIR)/parsec.o:	parsec.c ../include/egads.h ../include/egadsTypes.h \
		../include/egadsErrors.h
	$(CCOMP) -c $(COPTS) $(DEFINE) $(INCS) parsec.c -o $(TDIR)/parsec.o

$(TDIR)/parsef:		$(TDIR)/parsef.o $(LDIR)/libfgads.a $(LDIR)/$(SHLIB)
	$(FCOMP) -o $(TDIR)/parsef $(TDIR)/parsef.o -L$(LDIR) -lfgads -legads \
		-lstdc++

$(TDIR)/parsef.o:	parsef.f
	$(FCOMP) -c $(FOPTS) -I../include parsef.f -o $(TDIR)/parsef.o

clean:
	(cd $(TDIR); \
	rm testc.o testc testf.o testf parsec.o parsec parsef.o parsef )

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

$(TDIR)/fillet:	$(TDIR)/fillet.o $(LDIR)/$(SHLIB)
	$(CC) -o $(TDIR)/fillet $(TDIR)/fillet.o -L$(LDIR) -legads -lm

$(TDIR)/fillet.o:	fillet.c $(IDIR)/egads.h $(IDIR)/egadsTypes.h \
		$(IDIR)/egadsErrors.h
	$(CCOMP) -c $(COPTS) $(DEFINE) -I$(IDIR) fillet.c -o $(TDIR)/fillet.o

clean:
	-rm $(TDIR)/fillet.o $(TDIR)/fillet

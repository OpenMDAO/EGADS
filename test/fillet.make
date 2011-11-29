#
include ../include/$(GEM_ARCH)
LDIR = ../../$(GEM_ARCH)/lib
TDIR = ../../$(GEM_ARCH)/test

$(TDIR)/fillet:	$(TDIR)/fillet.o $(LDIR)/$(SHLIB)
	$(CC) -o $(TDIR)/fillet $(TDIR)/fillet.o -L$(LDIR) -legads -lm

$(TDIR)/fillet.o:	fillet.c ../include/egads.h ../include/egadsTypes.h \
		../include/egadsErrors.h
	$(CCOMP) -c $(COPTS) $(DEFINE) -I../include fillet.c -o $(TDIR)/fillet.o

clean:
	-rm $(TDIR)/fillet.o $(TDIR)/fillet

#
include ../include/$(GEM_ARCH)
LDIR = ../../$(GEM_ARCH)/lib
TDIR = ../../$(GEM_ARCH)/test

$(TDIR)/extrot:	$(TDIR)/extrot.o $(LDIR)/$(SHLIB)
	$(CC) -o $(TDIR)/extrot $(TDIR)/extrot.o -L$(LDIR) -legads -lm

$(TDIR)/extrot.o:	extrot.c ../include/egads.h ../include/egadsTypes.h \
		../include/egadsErrors.h
	$(CCOMP) -c $(COPTS) $(DEFINE) -I../include extrot.c -o $(TDIR)/extrot.o

clean:
	-rm $(TDIR)/extrot.o $(TDIR)/extrot

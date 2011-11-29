#
include ../include/$(GEM_ARCH)
LDIR = ../../$(GEM_ARCH)/lib
TDIR = ../../$(GEM_ARCH)/test

$(TDIR)/makeCone:	$(TDIR)/makeCone.o $(LDIR)/$(SHLIB)
	$(CC) -o $(TDIR)/makeCone $(TDIR)/makeCone.o -L$(LDIR) -legads -lm

$(TDIR)/makeCone.o:	makeCone.c ../include/egads.h ../include/egadsTypes.h \
		../include/egadsErrors.h
	$(CCOMP) -c $(COPTS) $(DEFINE) -I../include makeCone.c -o $(TDIR)/makeCone.o

clean:
	-rm $(TDIR)/makeCone.o $(TDIR)/makeCone

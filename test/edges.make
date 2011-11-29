#
include ../include/$(GEM_ARCH)
LDIR = ../../$(GEM_ARCH)/lib
TDIR = ../../$(GEM_ARCH)/test

$(TDIR)/edges:	$(TDIR)/edges.o $(LDIR)/$(SHLIB)
	$(CC) -o $(TDIR)/edges $(TDIR)/edges.o -L$(LDIR) -legads -lm

$(TDIR)/edges.o:	edges.c ../include/egads.h ../include/egadsTypes.h \
		../include/egadsErrors.h
	$(CCOMP) -c $(COPTS) $(DEFINE) -I../include edges.c -o $(TDIR)/edges.o

clean:
	-rm $(TDIR)/edges.o $(TDIR)/edges

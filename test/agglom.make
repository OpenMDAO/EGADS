#
include ../include/$(GEM_ARCH)
LDIR = ../../$(GEM_ARCH)/lib
TDIR = ../../$(GEM_ARCH)/test

$(TDIR)/agglom:	$(TDIR)/agglom.o $(LDIR)/$(SHLIB)
	$(CC) -o $(TDIR)/agglom $(TDIR)/agglom.o -L$(LDIR) -legads -lm

$(TDIR)/agglom.o:	agglom.c ../include/egads.h ../include/egadsTypes.h \
		../include/egadsErrors.h
	$(CCOMP) -c $(COPTS) $(DEFINE) -I../include agglom.c -o $(TDIR)/agglom.o

clean:
	-rm $(TDIR)/agglom.o $(TDIR)/agglom

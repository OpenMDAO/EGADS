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

$(TDIR)/agglom:	$(TDIR)/agglom.o $(LDIR)/$(SHLIB)
	$(CC) -o $(TDIR)/agglom $(TDIR)/agglom.o -L$(LDIR) -legads -lm

$(TDIR)/agglom.o:	agglom.c $(IDIR)/egads.h $(IDIR)/egadsTypes.h \
		$(IDIR)/egadsErrors.h
	$(CCOMP) -c $(COPTS) $(DEFINE) -I$(IDIR) agglom.c -o $(TDIR)/agglom.o

clean:
	-rm $(TDIR)/agglom.o $(TDIR)/agglom

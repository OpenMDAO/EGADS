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

$(TDIR)/edges:	$(TDIR)/edges.o $(LDIR)/$(SHLIB)
	$(CC) -o $(TDIR)/edges $(TDIR)/edges.o -L$(LDIR) -legads -lm

$(TDIR)/edges.o:	edges.c $(IDIR)/egads.h $(IDIR)/egadsTypes.h \
		$(IDIR)/egadsErrors.h
	$(CCOMP) -c $(COPTS) $(DEFINE) -I$(IDIR) edges.c -o $(TDIR)/edges.o

clean:
	-rm $(TDIR)/edges.o $(TDIR)/edges

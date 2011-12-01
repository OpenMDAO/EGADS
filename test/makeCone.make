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

$(TDIR)/makeCone:	$(TDIR)/makeCone.o $(LDIR)/$(SHLIB)
	$(CC) -o $(TDIR)/makeCone $(TDIR)/makeCone.o -L$(LDIR) -legads -lm

$(TDIR)/makeCone.o:	makeCone.c $(IDIR)/egads.h $(IDIR)/egadsTypes.h \
		$(IDIR)/egadsErrors.h
	$(CCOMP) -c $(COPTS) $(DEFINE) -I$(IDIR) makeCone.c -o $(TDIR)/makeCone.o

clean:
	-rm $(TDIR)/makeCone.o $(TDIR)/makeCone

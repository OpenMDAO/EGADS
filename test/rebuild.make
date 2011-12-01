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

$(TDIR)/rebuild:	$(TDIR)/rebuild.o $(LDIR)/$(SHLIB)
	$(CC) -o $(TDIR)/rebuild $(TDIR)/rebuild.o -L$(LDIR) -legads -lgv -lm

$(TDIR)/rebuild.o:	rebuild.c $(IDIR)/egads.h $(IDIR)/egadsTypes.h \
		$(IDIR)/egadsErrors.h
	$(CCOMP) -c $(COPTS) $(DEFINE) -I$(IDIR) rebuild.c \
		-o $(TDIR)/rebuild.o

clean:
	-rm $(TDIR)/rebuild.o $(TDIR)/rebuild

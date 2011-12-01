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

$(TDIR)/wireWing:	$(TDIR)/wireWing.o $(LDIR)/$(SHLIB)
	$(CC) -o $(TDIR)/wireWing $(TDIR)/wireWing.o -L$(LDIR) -legads -lm

$(TDIR)/wireWing.o:	wireWing.c $(IDIR)/egads.h $(IDIR)/egadsTypes.h \
		$(IDIR)/egadsErrors.h
	$(CCOMP) -c $(COPTS) $(DEFINE) -I$(IDIR) wireWing.c \
		-o $(TDIR)/wireWing.o

clean:
	-rm $(TDIR)/wireWing.o $(TDIR)/wireWing

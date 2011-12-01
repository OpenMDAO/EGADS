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

$(TDIR)/extrot:	$(TDIR)/extrot.o $(LDIR)/$(SHLIB)
	$(CC) -o $(TDIR)/extrot $(TDIR)/extrot.o -L$(LDIR) -legads -lm

$(TDIR)/extrot.o:	extrot.c $(IDIR)/egads.h $(IDIR)/egadsTypes.h \
		$(IDIR)/egadsErrors.h
	$(CCOMP) -c $(COPTS) $(DEFINE) -I$(IDIR) extrot.c -o $(TDIR)/extrot.o

clean:
	-rm $(TDIR)/extrot.o $(TDIR)/extrot

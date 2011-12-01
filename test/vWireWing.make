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

$(TDIR)/vWireWing:	$(TDIR)/vWireWing.o $(LDIR)/$(SHLIB) $(LDIR)/libgv.a
	$(CC) -o $(TDIR)/vWireWing $(TDIR)/vWireWing.o -L$(LDIR) -legads \
		-lgv $(GLIBS) -lm

$(TDIR)/vWireWing.o:	vWireWing.c $(IDIR)/egads.h $(IDIR)/egadsTypes.h \
		$(IDIR)/egadsErrors.h
	$(CCOMP) -c $(COPTS) $(DEFINE) -I$(IDIR) vWireWing.c \
		-o $(TDIR)/vWireWing.o

clean:
	-rm $(TDIR)/vWireWing.o $(TDIR)/vWireWing

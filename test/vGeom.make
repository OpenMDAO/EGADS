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

$(TDIR)/vGeom:		$(TDIR)/vGeom.o $(LDIR)/$(SHLIB) $(LDIR)/libgv.a
	$(CC) -o $(TDIR)/vGeom $(TDIR)/vGeom.o -L$(LDIR) -legads -lgv \
		$(GLIBS) -lm

$(TDIR)/vGeom.o:	vGeom.c $(IDIR)/egads.h $(IDIR)/egadsTypes.h \
		$(IDIR)/egadsErrors.h
	$(CCOMP) -c $(COPTS) $(DEFINE) -I$(IDIR) vGeom.c -o $(TDIR)/vGeom.o

clean:
	-rm $(TDIR)/vGeom.o $(TDIR)/vGeom

#
include ../include/$(GEM_ARCH)
LDIR = ../../$(GEM_ARCH)/lib
TDIR = ../../$(GEM_ARCH)/test

$(TDIR)/vGeom:		$(TDIR)/vGeom.o $(LDIR)/$(SHLIB) $(LDIR)/libgv.a
	$(CC) -o $(TDIR)/vGeom $(TDIR)/vGeom.o -L$(LDIR) -legads -lgv \
		$(GLIBS) -lm

$(TDIR)/vGeom.o:	vGeom.c ../include/egads.h ../include/egadsTypes.h \
		../include/egadsErrors.h
	$(CCOMP) -c $(COPTS) $(DEFINE) -I../include vGeom.c -o $(TDIR)/vGeom.o

clean:
	-rm $(TDIR)/vGeom.o $(TDIR)/vGeom

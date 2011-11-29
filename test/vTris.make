#
include ../include/$(GEM_ARCH)
LDIR = ../../$(GEM_ARCH)/lib
TDIR = ../../$(GEM_ARCH)/test

$(TDIR)/vTris:		$(TDIR)/vTris.o $(LDIR)/$(SHLIB) $(LDIR)/libfgads.a \
		$(LDIR)/libgv.a
	$(FCOMP) -o $(TDIR)/vTris $(TDIR)/vTris.o -L$(LDIR) -lfgads \
		-legads -lgv $(GLIBS) -lstdc++

$(TDIR)/vTris.o:	vTris.f ../include/egads.inc ../include/gv.inc
	$(FCOMP) -c $(FOPTS) -I../include vTris.f -o $(TDIR)/vTris.o

clean:
	-rm $(TDIR)/vTris.o $(TDIR)/vTris

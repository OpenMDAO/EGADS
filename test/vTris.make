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

$(TDIR)/vTris:		$(TDIR)/vTris.o $(LDIR)/$(SHLIB) $(LDIR)/libfgads.a \
		$(LDIR)/libgv.a
	$(FCOMP) -o $(TDIR)/vTris $(TDIR)/vTris.o -L$(LDIR) -lfgads \
		-legads -lgv $(GLIBS) -lstdc++

$(TDIR)/vTris.o:	vTris.f $(IDIR)/egads.inc $(IDIR)/gv.inc
	$(FCOMP) -c $(FOPTS) -I$(IDIR) vTris.f -o $(TDIR)/vTris.o

clean:
	-rm $(TDIR)/vTris.o $(TDIR)/vTris

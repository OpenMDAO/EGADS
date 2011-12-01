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

$(TDIR)/union:	$(TDIR)/union.o $(LDIR)/libfgads.a $(LDIR)/$(SHLIB)
	$(FCOMP) -o $(TDIR)/union $(TDIR)/union.o -L$(LDIR) -lfgads -legads \
		-lstdc++

$(TDIR)/union.o:	union.f
	$(FCOMP) -c $(FOPTS) -I$(IDIR) union.f -o $(TDIR)/union.o

clean:
	-rm $(TDIR)/union.o $(TDIR)/union

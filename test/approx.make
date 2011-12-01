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

$(TDIR)/approx:	$(TDIR)/approx.o $(LDIR)/libfgads.a $(LDIR)/$(SHLIB)
	$(FCOMP) -o $(TDIR)/approx $(TDIR)/approx.o -L$(LDIR) -lfgads \
		-legads -lstdc++

$(TDIR)/approx.o:	approx.f
	$(FCOMP) -c $(FOPTS) -I$(IDIR) approx.f -o $(TDIR)/approx.o

clean:
	-rm $(TDIR)/approx.o $(TDIR)/approx

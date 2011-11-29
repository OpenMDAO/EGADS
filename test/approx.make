#
include ../include/$(GEM_ARCH)
LDIR = ../../$(GEM_ARCH)/lib
TDIR = ../../$(GEM_ARCH)/test

$(TDIR)/approx:	$(TDIR)/approx.o $(LDIR)/libfgads.a $(LDIR)/$(SHLIB)
	$(FCOMP) -o $(TDIR)/approx $(TDIR)/approx.o -L$(LDIR) -lfgads \
		-legads -lstdc++

$(TDIR)/approx.o:	approx.f
	$(FCOMP) -c $(FOPTS) -I../include approx.f -o $(TDIR)/approx.o

clean:
	-rm $(TDIR)/approx.o $(TDIR)/approx

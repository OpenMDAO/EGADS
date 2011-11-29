#
include ../include/$(GEM_ARCH)
LDIR = ../../$(GEM_ARCH)/lib
TDIR = ../../$(GEM_ARCH)/test

$(TDIR)/union:	$(TDIR)/union.o $(LDIR)/libfgads.a $(LDIR)/$(SHLIB)
	$(FCOMP) -o $(TDIR)/union $(TDIR)/union.o -L$(LDIR) -lfgads -legads \
		-lstdc++

$(TDIR)/union.o:	union.f
	$(FCOMP) -c $(FOPTS) -I../include union.f -o $(TDIR)/union.o

clean:
	-rm $(TDIR)/union.o $(TDIR)/union

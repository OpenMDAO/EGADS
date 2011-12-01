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

$(TDIR)/closure:	$(TDIR)/closure.o $(LDIR)/libfgads.a $(LDIR)/$(SHLIB)
	$(FCOMP) -o $(TDIR)/closure $(TDIR)/closure.o -L$(LDIR) -lfgads \
		-legads -lstdc++

$(TDIR)/closure.o:	closure.f
	$(FCOMP) -c $(FOPTS) -I$(IDIR) closure.f -o $(TDIR)/closure.o

clean:
	-rm $(TDIR)/closure.o $(TDIR)/closure

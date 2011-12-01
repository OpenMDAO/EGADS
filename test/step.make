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

$(TDIR)/step:	$(TDIR)/step.o $(LDIR)/libfgads.a $(LDIR)/$(SHLIB)
	$(FCOMP) -o $(TDIR)/step $(TDIR)/step.o -L$(LDIR) -lfgads -legads \
		-lstdc++

$(TDIR)/step.o:	step.f
	$(FCOMP) -c $(FOPTS) -I$(IDIR) step.f -o $(TDIR)/step.o

clean:
	-rm $(TDIR)/step.o $(TDIR)/step

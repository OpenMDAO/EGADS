#
include ../include/$(GEM_ARCH)
LDIR = ../../$(GEM_ARCH)/lib
TDIR = ../../$(GEM_ARCH)/test

$(TDIR)/step:	$(TDIR)/step.o $(LDIR)/libfgads.a $(LDIR)/$(SHLIB)
	$(FCOMP) -o $(TDIR)/step $(TDIR)/step.o -L$(LDIR) -lfgads -legads \
		-lstdc++

$(TDIR)/step.o:	step.f
	$(FCOMP) -c $(FOPTS) -I../include step.f -o $(TDIR)/step.o

clean:
	-rm $(TDIR)/step.o $(TDIR)/step

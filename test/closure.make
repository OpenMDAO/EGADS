#
include ../include/$(GEM_ARCH)
LDIR = ../../$(GEM_ARCH)/lib
TDIR = ../../$(GEM_ARCH)/test

$(TDIR)/closure:	$(TDIR)/closure.o $(LDIR)/libfgads.a $(LDIR)/$(SHLIB)
	$(FCOMP) -o $(TDIR)/closure $(TDIR)/closure.o -L$(LDIR) -lfgads \
		-legads -lstdc++

$(TDIR)/closure.o:	closure.f
	$(FCOMP) -c $(FOPTS) -I../include closure.f -o $(TDIR)/closure.o

clean:
	-rm $(TDIR)/closure.o $(TDIR)/closure

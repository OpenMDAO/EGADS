#
include ../include/$(GEM_ARCH)
LDIR = ../../$(GEM_ARCH)/lib
TDIR = ../../$(GEM_ARCH)/test

$(TDIR)/vWireWing:	$(TDIR)/vWireWing.o $(LDIR)/$(SHLIB) $(LDIR)/libgv.a
	$(CC) -o $(TDIR)/vWireWing $(TDIR)/vWireWing.o -L$(LDIR) -legads \
		-lgv $(GLIBS) -lm

$(TDIR)/vWireWing.o:	vWireWing.c ../include/egads.h ../include/egadsTypes.h \
		../include/egadsErrors.h
	$(CCOMP) -c $(COPTS) $(DEFINE) -I../include vWireWing.c \
		-o $(TDIR)/vWireWing.o

clean:
	-rm $(TDIR)/vWireWing.o $(TDIR)/vWireWing

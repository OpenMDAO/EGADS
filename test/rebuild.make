#
include ../include/$(GEM_ARCH)
LDIR = ../../$(GEM_ARCH)/lib
TDIR = ../../$(GEM_ARCH)/test

$(TDIR)/rebuild:	$(TDIR)/rebuild.o $(LDIR)/$(SHLIB)
	$(CC) -o $(TDIR)/rebuild $(TDIR)/rebuild.o -L$(LDIR) -legads -lgv -lm

$(TDIR)/rebuild.o:	rebuild.c ../include/egads.h ../include/egadsTypes.h \
		../include/egadsErrors.h
	$(CCOMP) -c $(COPTS) $(DEFINE) -I../include rebuild.c \
		-o $(TDIR)/rebuild.o

clean:
	-rm $(TDIR)/rebuild.o $(TDIR)/rebuild

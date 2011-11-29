#
include ../include/$(GEM_ARCH)
LDIR = ../../$(GEM_ARCH)/lib
TDIR = ../../$(GEM_ARCH)/test

$(TDIR)/wireWing:	$(TDIR)/wireWing.o $(LDIR)/$(SHLIB)
	$(CC) -o $(TDIR)/wireWing $(TDIR)/wireWing.o -L$(LDIR) -legads -lm

$(TDIR)/wireWing.o:	wireWing.c ../include/egads.h ../include/egadsTypes.h \
		../include/egadsErrors.h
	$(CCOMP) -c $(COPTS) $(DEFINE) -I../include wireWing.c \
		-o $(TDIR)/wireWing.o

clean:
	-rm $(TDIR)/wireWing.o $(TDIR)/wireWing

#
include ../include/$(GEM_ARCH)
LDIR = ../../$(GEM_ARCH)/lib
TDIR = ../../$(GEM_ARCH)/test

$(TDIR)/intersect:	$(TDIR)/intersect.o $(LDIR)/$(SHLIB)
	$(CC) -o $(TDIR)/intersect $(TDIR)/intersect.o -L$(LDIR) -legads -lm

$(TDIR)/intersect.o:	intersect.c ../include/egads.h ../include/egadsTypes.h \
		../include/egadsErrors.h
	$(CCOMP) -c $(COPTS) $(DEFINE) -I../include intersect.c \
		-o $(TDIR)/intersect.o

clean:
	-rm $(TDIR)/intersect.o $(TDIR)/intersect

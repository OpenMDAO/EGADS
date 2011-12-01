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

$(TDIR)/intersect:	$(TDIR)/intersect.o $(LDIR)/$(SHLIB)
	$(CC) -o $(TDIR)/intersect $(TDIR)/intersect.o -L$(LDIR) -legads -lm

$(TDIR)/intersect.o:	intersect.c $(IDIR)/egads.h $(IDIR)/egadsTypes.h \
		$(IDIR)/egadsErrors.h
	$(CCOMP) -c $(COPTS) $(DEFINE) -I$(IDIR) intersect.c \
		-o $(TDIR)/intersect.o

clean:
	-rm $(TDIR)/intersect.o $(TDIR)/intersect

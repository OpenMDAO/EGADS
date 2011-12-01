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

$(TDIR)/circle:	$(TDIR)/circle.o $(LDIR)/$(SHLIB)
	$(CC) -o $(TDIR)/circle $(TDIR)/circle.o -L$(LDIR) -legads -lm

$(TDIR)/circle.o:	circle.c $(IDIR)/egads.h $(IDIR)/egadsTypes.h \
		$(IDIR)/egadsErrors.h
	$(CCOMP) -c $(COPTS) $(DEFINE) -I$(IDIR) circle.c -o $(TDIR)/circle.o

clean:
	-rm $(TDIR)/circle.o $(TDIR)/circle

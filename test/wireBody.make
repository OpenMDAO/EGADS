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

$(TDIR)/wireBody:	$(TDIR)/wireBody.o $(LDIR)/$(SHLIB)
	$(CC) -o $(TDIR)/wireBody $(TDIR)/wireBody.o -L$(LDIR) -legads -lm

$(TDIR)/wireBody.o:	wireBody.c $(IDIR)/egads.h $(IDIR)/egadsTypes.h \
		$(IDIR)/egadsErrors.h
	$(CCOMP) -c $(COPTS) $(DEFINE) -I$(IDIR) wireBody.c -o $(TDIR)/wireBody.o

clean:
	-rm $(TDIR)/wireBody.o $(TDIR)/wireBody

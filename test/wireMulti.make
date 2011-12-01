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

$(TDIR)/wireMulti:	$(TDIR)/wireMulti.o $(LDIR)/$(SHLIB)
	$(CC) -o $(TDIR)/wireMulti $(TDIR)/wireMulti.o -L$(LDIR) -legads -lm

$(TDIR)/wireMulti.o:	wireMulti.c $(IDIR)/egads.h $(IDIR)/egadsTypes.h \
		$(IDIR)/egadsErrors.h
	$(CCOMP) -c $(COPTS) $(DEFINE) -I$(IDIR) wireMulti.c \
		-o $(TDIR)/wireMulti.o

clean:
	-rm $(TDIR)/wireMulti.o $(TDIR)/wireMulti

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

$(TDIR)/xform:	$(TDIR)/xform.o $(LDIR)/$(SHLIB)
	$(CC) -o $(TDIR)/xform $(TDIR)/xform.o -L$(LDIR) -legads -lm

$(TDIR)/xform.o:	xform.c $(IDIR)/egads.h $(IDIR)/egadsTypes.h \
		$(IDIR)/egadsErrors.h
	$(CCOMP) -c $(COPTS) $(DEFINE) -I$(IDIR) xform.c -o $(TDIR)/xform.o

clean:
	-rm $(TDIR)/xform.o $(TDIR)/xform

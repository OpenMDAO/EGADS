#
include ../include/$(GEM_ARCH)
LDIR = ../../$(GEM_ARCH)/lib
TDIR = ../../$(GEM_ARCH)/test

$(TDIR)/xform:	$(TDIR)/xform.o $(LDIR)/$(SHLIB)
	$(CC) -o $(TDIR)/xform $(TDIR)/xform.o -L$(LDIR) -legads -lm

$(TDIR)/xform.o:	xform.c ../include/egads.h ../include/egadsTypes.h \
		../include/egadsErrors.h
	$(CCOMP) -c $(COPTS) $(DEFINE) -I../include xform.c -o $(TDIR)/xform.o

clean:
	-rm $(TDIR)/xform.o $(TDIR)/xform

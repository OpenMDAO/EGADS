#
include ../include/$(GEM_ARCH)
LDIR = ../../$(GEM_ARCH)/lib
TDIR = ../../$(GEM_ARCH)/test

$(TDIR)/wireBody:	$(TDIR)/wireBody.o $(LDIR)/$(SHLIB)
	$(CC) -o $(TDIR)/wireBody $(TDIR)/wireBody.o -L$(LDIR) -legads -lm

$(TDIR)/wireBody.o:	wireBody.c ../include/egads.h ../include/egadsTypes.h \
		../include/egadsErrors.h
	$(CCOMP) -c $(COPTS) $(DEFINE) -I../include wireBody.c -o $(TDIR)/wireBody.o

clean:
	-rm $(TDIR)/wireBody.o $(TDIR)/wireBody

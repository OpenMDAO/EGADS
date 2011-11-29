#
include ../include/$(GEM_ARCH)
LDIR = ../../$(GEM_ARCH)/lib
TDIR = ../../$(GEM_ARCH)/test

$(TDIR)/circle:	$(TDIR)/circle.o $(LDIR)/$(SHLIB)
	$(CC) -o $(TDIR)/circle $(TDIR)/circle.o -L$(LDIR) -legads -lm

$(TDIR)/circle.o:	circle.c ../include/egads.h ../include/egadsTypes.h \
		../include/egadsErrors.h
	$(CCOMP) -c $(COPTS) $(DEFINE) -I../include circle.c -o $(TDIR)/circle.o

clean:
	-rm $(TDIR)/circle.o $(TDIR)/circle

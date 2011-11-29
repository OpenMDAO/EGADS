#
include ../include/$(GEM_ARCH)
LDIR = ../../$(GEM_ARCH)/lib
TDIR = ../../$(GEM_ARCH)/test

$(TDIR)/wireMulti:	$(TDIR)/wireMulti.o $(LDIR)/$(SHLIB)
	$(CC) -o $(TDIR)/wireMulti $(TDIR)/wireMulti.o -L$(LDIR) -legads -lm

$(TDIR)/wireMulti.o:	wireMulti.c ../include/egads.h ../include/egadsTypes.h \
		../include/egadsErrors.h
	$(CCOMP) -c $(COPTS) $(DEFINE) -I../include wireMulti.c \
		-o $(TDIR)/wireMulti.o

clean:
	-rm $(TDIR)/wireMulti.o $(TDIR)/wireMulti

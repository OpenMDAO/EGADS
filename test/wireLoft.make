#
include ../include/$(GEM_ARCH)
LDIR = ../../$(GEM_ARCH)/lib
TDIR = ../../$(GEM_ARCH)/test

$(TDIR)/wireLoft:	$(TDIR)/wireLoft.o $(LDIR)/$(SHLIB)
	$(CC) -o $(TDIR)/wireLoft $(TDIR)/wireLoft.o -L$(LDIR) -legads -lm

$(TDIR)/wireLoft.o:	wireLoft.c ../include/egads.h ../include/egadsTypes.h \
		../include/egadsErrors.h
	$(CCOMP) -c $(COPTS) $(DEFINE) -I../include wireLoft.c \
		-o $(TDIR)/wireLoft.o

clean:
	-rm $(TDIR)/wireLoft.o $(TDIR)/wireLoft

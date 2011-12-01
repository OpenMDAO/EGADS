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

$(TDIR)/wireLoft:	$(TDIR)/wireLoft.o $(LDIR)/$(SHLIB)
	$(CC) -o $(TDIR)/wireLoft $(TDIR)/wireLoft.o -L$(LDIR) -legads -lm

$(TDIR)/wireLoft.o:	wireLoft.c $(IDIR)/egads.h $(IDIR)/egadsTypes.h \
		$(IDIR)/egadsErrors.h
	$(CCOMP) -c $(COPTS) $(DEFINE) -I$(IDIR) wireLoft.c \
		-o $(TDIR)/wireLoft.o

clean:
	-rm $(TDIR)/wireLoft.o $(TDIR)/wireLoft

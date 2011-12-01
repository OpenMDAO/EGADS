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

default:	$(TDIR)/vTess $(TDIR)/vTesstatic

$(TDIR)/vTess:		$(TDIR)/vTess.o $(LDIR)/$(SHLIB) $(LDIR)/libgv.a
	$(CC) -o $(TDIR)/vTess $(TDIR)/vTess.o -L$(LDIR) -legads -lgv \
		$(GLIBS) -lm

$(TDIR)/vTesstatic:	$(TDIR)/vTess.o $(LDIR)/libegadstatic.a $(LDIR)/libgv.a
	$(CC) -o $(TDIR)/vTesstatic $(TDIR)/vTess.o $(LIBPATH) -legadstatic \
		$(LIBS) -lgv $(GLIBS) -lm

$(TDIR)/vTess.o:	vTess.c $(IDIR)/egads.h $(IDIR)/egadsTypes.h \
		$(IDIR)/egadsErrors.h
	$(CCOMP) -c $(COPTS) $(DEFINE) -I$(IDIR) vTess.c -o $(TDIR)/vTess.o

clean:
	-rm $(TDIR)/vTess.o $(TDIR)/vTess

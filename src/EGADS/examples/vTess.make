#
include ../include/$(GEM_ARCH)
IDIR = ../include
LDIR = $(GEM_ROOT)/lib
BDIR = $(GEM_ROOT)/bin

default:	$(BDIR)/vTess $(BDIR)/vTesstatic

$(BDIR)/vTess:	vTess.o retessFaces.o $(LDIR)/libwsserver.a
	$(CC) -o $(BDIR)/vTess vTess.o retessFaces.o -L$(LDIR) \
		-lwsserver -legads -lpthread -lz -lm

$(BDIR)/vTesstatic:	vTess.o retessFaces.o $(LDIR)/libwsserver.a \
			$(LDIR)/libegadstatic.a
	$(CC) -o $(BDIR)/vTesstatic vTess.o retessFaces.o \
		-L$(LDIR) -lwsserver -legadstatic $(LIBPATH) $(LIBS) \
		-lpthread -lz -lm

vTess.o:	vTess.c $(IDIR)/egads.h $(IDIR)/egadsTypes.h \
			$(IDIR)/egadsErrors.h $(IDIR)/wsserver.h
	$(CCOMP) -c $(COPTS) $(DEFINE) -I$(IDIR) vTess.c

retessFaces.o:	../util/retessFaces.c $(IDIR)/egads.h $(IDIR)/egadsTypes.h \
		$(IDIR)/egadsErrors.h
	$(CCOMP) -c $(COPTS) $(DEFINE) -I$(IDIR) ../util/retessFaces.c

clean:
	-rm vTess.o retessFaces.o

cleanall:
	-rm vTess.o retessFaces.o $(BDIR)/vTess $(BDIR)/vTesstatic

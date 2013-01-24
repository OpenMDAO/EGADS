#
include ../include/$(GEM_ARCH)
IDIR  = ../include
UDIR  = ../util
LDIR  = $(GEM_ROOT)/lib

OBJSP = egadsGeom.o egadsHLevel.o egadsTopo.o  egadsCopy.o egadsIO.o
OBJS  = egadsBase.o egadsMemory.o egadsAttrs.o egadsTess.o egadsTris.o \
	egadsQuads.o
FOBJS = fgadsBase.o fgadsMemory.o fgadsAttrs.o fgadsTess.o \
	fgadsGeom.o fgadsHLevel.o fgadsTopo.o

default:	$(LDIR)/$(SHLIB) $(LDIR)/libegadstatic.a $(LDIR)/libfgads.a \
		$(LDIR)/libemp.a

$(LDIR)/libegads.so:		$(OBJSP) $(OBJS) emp.o
	-rm $(LDIR)/libegads.so
	$(CCOMP) -shared -o $(LDIR)/libegads.so $(OBJSP) $(OBJS) emp.o \
		$(LIBPATH) $(LIBS)

$(LDIR)/libegads.dylib:		$(OBJSP) $(OBJS) emp.o
	-rm $(LDIR)/libegads.dylib
	$(CCOMP) -dynamiclib -o $(LDIR)/libegads.dylib $(OBJSP) \
		$(OBJS) emp.o $(LIBPATH) $(LIBS) -undefined dynamic_lookup \
		-install_name libegads.dylib -compatibility_version $(CASREV) \
		-current_version $(EGREV)

$(LDIR)/libegadstatic.a:	$(OBJSP) $(OBJS) emp.o
	-rm $(LDIR)/libegadstatic.a
	ar $(LOPTS) $(LDIR)/libegadstatic.a $(OBJS) $(OBJSP) emp.o
	$(RANLB)

$(LDIR)/libfgads.a:		$(FOBJS)
	-rm $(LDIR)/libfgads.a
	ar $(LOPTS) $(LDIR)/libfgads.a $(FOBJS)
	$(RANLB)

$(LDIR)/libemp.a:		emp.o
	-rm $(LDIR)/libemp.a
	ar $(LOPTS) $(LDIR)/libemp.a emp.o
	$(RANLB)

$(OBJS):	$(IDIR)/egadsErrors.h egadsInternals.h \
		$(IDIR)/egadsTypes.h egadsTris.h
.c.o:
	$(CCOMP) -c $(COPTS) $(DEFINE) -I$(IDIR) $<

$(FOBJS):	$(IDIR)/egadsErrors.h egadsInternals.h \
		$(IDIR)/egadsTypes.h 
.c.o:
	$(CCOMP) -c $(COPTS) $(DEFINE) -I$(IDIR) $<

$(OBJSP):	$(IDIR)/egadsErrors.h egadsOCC.h egadsInternals.h \
		$(IDIR)/egadsTypes.h egadsClasses.h
.cpp.o:
	$(CC) -c $(CPPOPT) $(DEFINE) $(INCS) -I$(IDIR) $<

emp.o:		$(UDIR)/emp.c
	$(CCOMP) -c $(COPTS) $(DEFINE) -I$(IDIR) $(UDIR)/emp.c

clean:
	-rm $(FOBJS) $(OBJSP) $(OBJS) emp.o

cleanall:
	-rm $(LDIR)/libegads* $(LDIR)/libfgads.a $(LDIR)/libemp.a
	-rm $(FOBJS) $(OBJSP) $(OBJS) emp.o

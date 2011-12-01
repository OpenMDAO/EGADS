#
include ../include/$(GEM_ARCH)
IDIR  = $(CURDIR)/../include
ifdef GEM_BLOC
ODIR  = $(GEM_BLOC)/obj
LDIR  = $(GEM_BLOC)/lib
else
ODIR  = ../../$(GEM_ARCH)/obj
LDIR  = ../../$(GEM_ARCH)/lib
endif

VPATH = $(ODIR)

OBJSP = egadsGeom.o egadsHLevel.o egadsTopo.o  egadsCopy.o egadsIO.o
OBJS  = egadsBase.o egadsMemory.o egadsAttrs.o egadsTess.o egadsTris.o \
	egadsQuads.o
FOBJS = fgadsBase.o fgadsMemory.o fgadsAttrs.o fgadsTess.o \
	fgadsGeom.o fgadsHLevel.o fgadsTopo.o  fgv.o

default:	$(LDIR)/$(SHLIB) $(LDIR)/libegadstatic.a $(LDIR)/libfgads.a

$(LDIR)/libegads.so:		$(OBJSP) $(OBJS)
	-rm $(LDIR)/libegads.so
	(cd $(ODIR); $(CCOMP) -shared -o $(LDIR)/libegads.so $(OBJSP) $(OBJS) \
		$(LIBPATH) $(LIBS) )

$(LDIR)/libegads.dylib:		$(OBJSP) $(OBJS)
	-rm $(LDIR)/libegads.dylib
	(cd $(ODIR); \
		$(CCOMP) -dynamiclib -o $(LDIR)/libegads.dylib $(OBJSP) \
			$(OBJS) $(LIBPATH) $(LIBS) -undefined dynamic_lookup \
			-install_name libegads.dylib \
			-compatibility_version $(CASREV) \
			-current_version $(EGREV) )

$(LDIR)/libegadstatic.a:	$(OBJSP) $(OBJS)
	-rm $(LDIR)/libegadstatic.a
	(cd $(ODIR); ar $(LOPTS) $(LDIR)/libegadstatic.a $(OBJS) $(OBJSP); \
		$(RANLB) )

$(LDIR)/libfgads.a:		$(FOBJS)
	-rm $(LDIR)/libfgads.a
	(cd $(ODIR); ar $(LOPTS) $(LDIR)/libfgads.a $(FOBJS); $(RANLB) )

$(OBJS):	$(IDIR)/egadsErrors.h egadsInternals.h \
		$(IDIR)/egadsTypes.h egadsTris.h
.c.o:
	$(CCOMP) -c $(COPTS) $(DEFINE) -I$(IDIR) $< -o $(ODIR)/$@

$(FOBJS):	$(IDIR)/egadsErrors.h egadsInternals.h \
		$(IDIR)/egadsTypes.h 
.c.o:
	$(CCOMP) -c $(COPTS) $(DEFINE) -I$(IDIR) $< -o $(ODIR)/$@

$(OBJSP):	$(IDIR)/egadsErrors.h egadsOCC.h egadsInternals.h \
		$(IDIR)/egadsTypes.h egadsClasses.h
.cpp.o:
	$(CC) -c $(CPPOPT) $(DEFINE) $(INCS) -I$(IDIR) $< -o $(ODIR)/$@

clean:
	(cd $(ODIR); rm $(FOBJS) $(OBJSP) $(OBJS) )

cleanall:
	-rm $(LDIR)/libegads.a $(LDIR)/libfgads.a
	(cd $(ODIR); rm $(FOBJS) $(OBJSP) $(OBJS) )

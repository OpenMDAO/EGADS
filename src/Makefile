#
include ../include/$(GEM_ARCH)
ODIR  = ../../$(GEM_ARCH)/obj
LDIR  = ../../$(GEM_ARCH)/lib

VPATH = $(ODIR)

OBJSP = egadsGeom.o egadsHLevel.o egadsTopo.o  egadsCopy.o egadsIO.o
OBJS  = egadsBase.o egadsMemory.o egadsAttrs.o egadsTess.o egadsTris.o \
	egadsQuads.o
FOBJS = fgadsBase.o fgadsMemory.o fgadsAttrs.o fgadsTess.o \
	fgadsGeom.o fgadsHLevel.o fgadsTopo.o  fgv.o

default:	$(LDIR)/$(SHLIB) $(LDIR)/libegadstatic.a $(LDIR)/libfgads.a

$(LDIR)/libegads.so:		$(OBJSP) $(OBJS)
	-rm $(LDIR)/libegads.so
	(cd $(ODIR); $(CCOMP) -shared -o ../lib/libegads.so $(OBJSP) $(OBJS) \
		$(LIBPATH) $(LIBS) )

$(LDIR)/libegads.dylib:		$(OBJSP) $(OBJS)
	-rm $(LDIR)/libegads.dylib
	(cd $(ODIR); \
		$(CCOMP) -dynamiclib -o ../lib/libegads.dylib $(OBJSP) $(OBJS) \
			$(LIBPATH) $(LIBS) -undefined dynamic_lookup \
			-install_name libegads.dylib \
			-compatibility_version $(CASREV) \
			-current_version $(EGREV) )

$(LDIR)/libegadstatic.a:	$(OBJSP) $(OBJS)
	-rm $(LDIR)/libegadstatic.a
	(cd $(ODIR); ar $(LOPTS) ../lib/libegadstatic.a $(OBJS) $(OBJSP); $(RANLB) )

$(LDIR)/libfgads.a:		$(FOBJS)
	-rm $(LDIR)/libfgads.a
	(cd $(ODIR); ar $(LOPTS) ../lib/libfgads.a $(FOBJS); $(RANLB) )

$(OBJS):	../include/egadsErrors.h egadsInternals.h \
		../include/egadsTypes.h egadsTris.h
.c.o:
	$(CCOMP) -c $(COPTS) $(DEFINE) -I../include $< -o $(ODIR)/$@

$(FOBJS):	../include/egadsErrors.h egadsInternals.h \
		../include/egadsTypes.h 
.c.o:
	$(CCOMP) -c $(COPTS) $(DEFINE) -I../include $< -o $(ODIR)/$@

$(OBJSP):	../include/egadsErrors.h egadsOCC.h egadsInternals.h \
		../include/egadsTypes.h egadsClasses.h
.cpp.o:
	$(CC) -c $(CPPOPT) $(DEFINE) $(INCS) -I../include $< -o $(ODIR)/$@

clean:
	(cd $(ODIR); rm $(FOBJS) $(OBJSP) $(OBJS) )

cleanall:
	-rm $(LDIR)/libegads.a $(LDIR)/libfgads.a
	(cd $(ODIR); rm $(FOBJS) $(OBJSP) $(OBJS) )

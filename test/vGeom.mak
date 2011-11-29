#
!include ../include/$(GEM_ARCH)
LDIR = ..\..\$(GEM_ARCH)\lib
TDIR = ..\..\$(GEM_ARCH)\test

default:	$(TDIR)\vGeom.exe $(TDIR)\vGeomStatic.exe

$(TDIR)\vGeom.exe:	$(TDIR)\vGeom.obj $(LDIR)\egads.lib $(LDIR)\gv.lib
	cl /Fe$(TDIR)\vGeom.exe $(TDIR)\vGeom.obj $(LIBPTH) egads.lib gv.lib \
		$(GLIBS)
	$(MCOMP) /manifest $(TDIR)\vGeom.exe.manifest \
		/outputresource:$(TDIR)\vGeom.exe;1

$(TDIR)\vGeomStatic.exe:	$(TDIR)\vGeom.obj $(LDIR)\egadstatic.lib \
				$(LDIR)\gv.lib
	cl /Fe$(TDIR)\vGeomStatic.exe $(TDIR)\vGeom.obj $(LIBPTH) \
		egadstatic.lib gv.lib $(LIBS) $(GLIBS)
	$(MCOMP) /manifest $(TDIR)\vGeomStatic.exe.manifest \
		/outputresource:$(TDIR)\vGeomStatic.exe;1

$(TDIR)\vGeom.obj:	vGeom.c ..\include\egads.h ..\include\egadsTypes.h \
		..\include\egadsErrors.h
	cl /c $(COPTS) $(DEFINE) -I../include vGeom.c /Fo$(TDIR)\vGeom.obj

clean:
	cd $(TDIR)
	-del vGeom.obj vGeom.exe *.manifest *.lib *.exp
	cd ..\..\trunk\test

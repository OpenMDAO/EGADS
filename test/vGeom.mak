#
!include ..\include\$(GEM_ARCH)
SDIR = $(MAKEDIR)
IDIR = $(SDIR)\..\include
!IFNDEF GEM_BLOC
LDIR = ..\..\$(GEM_ARCH)\lib
TDIR = ..\..\$(GEM_ARCH)\test
!ELSE
LDIR = $(GEM_BLOC)\lib
TDIR = $(GEM_BLOC)\test
!ENDIF

default:	$(TDIR)\vGeom.exe $(TDIR)\vGeomStatic.exe

$(TDIR)\vGeom.exe:	$(TDIR)\vGeom.obj $(LDIR)\egads.lib $(LDIR)\gv.lib
	cl /Fe$(TDIR)\vGeom.exe $(TDIR)\vGeom.obj $(LIBPTH) egads.lib \
		$(GLIBS)
	$(MCOMP) /manifest $(TDIR)\vGeom.exe.manifest \
		/outputresource:$(TDIR)\vGeom.exe;1

$(TDIR)\vGeomStatic.exe:	$(TDIR)\vGeom.obj $(LDIR)\egadstatic.lib \
				$(LDIR)\gv.lib
	cl /Fe$(TDIR)\vGeomStatic.exe $(TDIR)\vGeom.obj $(LIBPTH) \
		egadstatic.lib $(LIBS) $(GLIBS)
	$(MCOMP) /manifest $(TDIR)\vGeomStatic.exe.manifest \
		/outputresource:$(TDIR)\vGeomStatic.exe;1

$(TDIR)\vGeom.obj:	vGeom.c $(IDIR)\egads.h $(IDIR)\egadsTypes.h \
		$(IDIR)\egadsErrors.h
	cl /c $(COPTS) $(DEFINE) -I../include vGeom.c /Fo$(TDIR)\vGeom.obj

clean:
	cd $(TDIR)
	-del vGeom.obj vGeom.exe vGeomStatic.exe *.manifest *.lib *.exp
	cd $(SDIR)

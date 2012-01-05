#
!include ..\include\$(GEM_ARCH)
SDIR = $(MAKEDIR)
IDIR = $(SDIR)/../include
!IFDEF GEM_BLOC
LDIR = $(GEM_BLOC)/lib
TDIR = $(GEM_BLOC)/test
!ELSE
LDIR = ..\..\$(GEM_ARCH)/lib
TDIR = ..\..\$(GEM_ARCH)/test
!ENDIF

$(TDIR)\vTess.exe:	$(TDIR)\vTess.obj $(LDIR)\egads.lib $(LDIR)\gv.lib
	cl /Fe$(TDIR)\vTess.exe $(TDIR)\vTess.obj $(LIBPTH) egads.lib \
		$(GLIBS)
	$(MCOMP) /manifest $(TDIR)\vTess.exe.manifest \
		/outputresource:$(TDIR)\vTess.exe;1

$(TDIR)\vTess.obj:	vTess.c $(IDIR)\egads.h $(IDIR)\egadsTypes.h \
		$(IDIR)\egadsErrors.h
	cl /c $(COPTS) $(DEFINE) -I../include vTess.c /Fo$(TDIR)\vTess.obj

clean:
        cd $(TDIR)
        -del vTess.obj vTess.exe *.manifest *.lib *.exp
        cd $(SDIR)

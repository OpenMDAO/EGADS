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

default:	$(TDIR)\testc.exe $(TDIR)\testf.exe $(TDIR)\parsec.exe \
		$(TDIR)\parsef.exe

$(TDIR)\testc.exe:	$(TDIR)\testc.obj $(LDIR)\egads.lib
	cl /Fe$(TDIR)\testc.exe $(TDIR)\testc.obj $(LIBPTH) egads.lib
	$(MCOMP) /manifest $(TDIR)\testc.exe.manifest \
		/outputresource:$(TDIR)\testc.exe;1

$(TDIR)\testc.obj:	testc.c $(IDIR)\egads.h $(IDIR)\egadsTypes.h \
		$(IDIR)\egadsErrors.h
	cl /c $(COPTS) $(DEFINE) -I$(IDIR) testc.c /Fo$(TDIR)\testc.obj

$(TDIR)\testf.exe:	$(TDIR)\testf.obj $(LDIR)\fgads.lib $(LDIR)\egads.lib
	ifort /traceback /Fe$(TDIR)\testf.exe $(TDIR)\testf.obj $(LIBPTH) \
		fgads.lib egads.lib
#	cl /MD /Fe$(TDIR)\testf.exe $(TDIR)\testf.obj $(LIBPTH) fgads.lib \
#		egads.lib libifcoremd.lib \
#		libifportmd.lib libmmd.lib /entry:mainCRTStartup
	$(MCOMP) /manifest $(TDIR)\testf.exe.manifest \
		/outputresource:$(TDIR)\testf.exe;1

$(TDIR)\testf.obj:	testf.f
	ifort /c $(FOPTS) -I$(IDIR) testf.f /Fo$(TDIR)\testf.obj

$(TDIR)\parsec.exe:		$(TDIR)\parsec.obj $(LDIR)\egads.lib
	cl /Fe$(TDIR)\parsec.exe $(TDIR)\parsec.obj $(LIBPTH) egads.lib
	$(MCOMP) /manifest $(TDIR)\parsec.exe.manifest \
		/outputresource:$(TDIR)\parsec.exe;1

$(TDIR)\parsec.obj:	parsec.c $(IDIR)\egads.h $(IDIR)\egadsTypes.h \
		$(IDIR)\egadsErrors.h
	cl /c $(COPTS) $(DEFINE) -I$(IDIR) parsec.c /Fo$(TDIR)\parsec.obj

$(TDIR)\parsef.exe:	$(TDIR)\parsef.obj $(LDIR)\fgads.lib $(LDIR)\egads.lib
	ifort /traceback /Fe$(TDIR)\parsef.exe $(TDIR)\parsef.obj $(LIBPTH) \
		fgads.lib egads.lib
	$(MCOMP) /manifest $(TDIR)\parsef.exe.manifest \
		/outputresource:$(TDIR)\parsef.exe;1

$(TDIR)\parsef.obj:	parsef.f
	ifort /c $(FOPTS) -I$(IDIR) parsef.f /Fo$(TDIR)\parsef.obj

clean:
	cd $(TDIR)
	-del testc.obj testc.exe testf.obj testf.exe *.manifest \
		 parsec.obj parsec.exe parsef.obj parsef.exe *.lib *.exp
	cd $(SDIR)

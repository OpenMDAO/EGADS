			EGADS REVISION 0.86 DISTRIBUTION
		  Electronic Geometry Aircraft Design System


1. Prerequisites

	The only significant prerequisite for EGADS is OpenCASCADE at release
6.3.0 or greater (which now includes the OpenCASCADE Community Edition -- OCE).
This can be found at http://www.opencascade.org/getocc/download/loadocc or
https://github.com/tpaviot/oce. Prebuilt versions are available at these sites
for Windows using various versions of Visual Studio and MAC OSX at 64-bits. 
Any other configuration must be built from source (unless you are using a 
Debian variant of LINUX, such as Ubuntu, where there are available prebuilt 
packages as part of the LINUX distribution).

1.1 EGADS Source Distribution Layout

README.txt - this file
docs       - documentation 
include    - headers used by EGADS and those used to build EGADS apps
src        - source files
test       - test and example code

1.2 EGADS Binary Distribution

	Some test and example codes use graphics through a non-Open Source
library: GV (the Geometry Viewer). The library is made available as part
of the current EGADS distribution but will disappear in the future. A
separate download (in the form of a g'zipped tarball) is required which 
when unpacked also provides a target for the EGADS build. The tar image 
"EGADSbin.tgz" contains the following directories:

DARWIN   - binary target for OSX
DARWIN64 - binary target for OSX 64-bit
LINUX    - binary target for LINUX
LINUX64  - binary target for LINUX 64-bit
WIN32    - binary target for 32-bit Windows
WIN64    - binary target for 64-bit Windows

Each directory has subdirectories "lib", "obj" and "test".


2. Building EGADS

	EGADS does not use the typical "configure-make-install" procedure as
seen for many LINUX packages. Simple Make (or NMake on Windows) files are used.
These are driven by a small number of environment variables as seen below.

2.1 Build Environment Variables

	GEM_ARCH - tells EGADS the base-level OS/architecture for the target:
			DARWIN   - MAC OSX 10.5 or greater at 32-bits
			DARWIN64 - MAC OSX 10.5 or greater at 64-bits
			LINUX    - LINUX at 32-bits
			LINUX64  - LINUX at 64-bits
			WIN32    - XP, Vista or Windows7 at 32-bits
			WIN64    - XP, Vista or Windows7 at 64-bits
        GEM_BLOC - the build location (path) for the appropriate binary 
		   target. This must must contain the "lib", "obj" and 
		   "test" subdirectories for GEM_ARCH.
	CASROOT  - the install path to find OpenCASCADE (which typically has
		   a "bin", "inc", "lib" and "src" subdirectories). For 
		   Debian installs this may be "/usr/include/opencascade".
	CASARCH  - this is the string that OpenCASCADE internally uses for
		   the system architecture and is usually a subdirectory
		   of CASROOT. On Debian machines this is usually "Linux".
	CASREV   - the major.minor revision of OpenCASCADE. For example:
		   "6.3" or "6.5".

2.2 Notes on building FORTRAN bindings:

	Both gfortran & ifort (the Intel FORTRAN compiler) are supported
for LINUX and the MAC, but only ifort support is available under Windows.
The build of EGADS is independent of the FORTRAN compiler used (if any).
The test codes (in the "test"  EGADS directory) that use FORTRAN must 
have a compiler. The assumption is that the complier is gfortran (unless
Windows). This can be simply modified by editing the configuration files
in the EGADS "include" subdirectory. These files have the name used by 
the environment variable GEM_ARCH. There are 2 sets of assignments for 
both FCOMP and FOPTS where the one for ifort is commented out.

2.3 The Build

	Once the above is all set, just go into the EGADS "src" directory 
and type: make.
	For Windows, there are no MSVS project files. It is assumed that a
"command window" is open and the environment has been setup for the 
appropriate compiler(s). There is a "make.bat" that executes "nmake".

2.4 The Tests

	The small test examples can be made by executing "make" (or "nmake"
when using Visual Studio) from within the "test" EGADS directory. This 
can be simply done by: "make -f XYZ.make" (or "nmake -f XYZ.mak" at a
command prompt under Windows). Where XYZ is the name of any of the 
test/example codes.

2.5 Windows & Visual Studio

	Building on Windows is challenging with prebuilt binaries. This is
because there are no "system libraries" and Microsoft (in its infinite
wisdom) changes the run-time components for each release of Visual Studio. 
One can mix components (with care) if things are primarily in the form of 
DLLs. Therefore it is usually important to match the compiler (and compiler
options) with any larger build. The build for 32-bit Windows will work with 
OpenCASCADE 6.3 as delivered from the OpenCASCADE site (this uses MSVS 2003, 
also known as Version 7.1). Because the prebuilt package from OpenCASCADE for 
6.5 uses MSVS 2005 (Version 8.0) which does not work on Windows7, the example 
build uses OCE for MSVS 2008 (Version 9.0).
	Please contact Bob Haimes (at the coordinates below) for Geometry
Viewer Libraries that may be required for other versions of Visual Studio.


3. Running an EGADS Application

	Before an EGADS application can be executed, the dynamic loader must
be told where to find the required components. This includes the paths for
both OpenCASCADE and EGADS. The dynamic loader is informed where to look by 
environment variable (but the name differs depending on the OS):

	MAC OSX		DYLD_LIBRARY_PATH (colon is the separator)
	LINUX		LD_LIBRARY_PATH   (colon is the separator)
	WIN32/64	PATH          (semicolon is the separator)

For example, this can be done on Windows at the "command window" or for a
"bat" file with the command:

  % set PATH=%CASROOT%\%CASARCH%\bin;%GEM_BLOC%\lib;%PATH%


4. Coordinates:

	Bob Haimes
	(617) 253-7518
	haimes@mit.edu

		EGADS: Engineering Geometry Aerospace Design System 
					Rev 1.0


1. Prerequisites

	The most significant prerequisite for this software is OpenCASCADE at 
release 6.3.0 or greater (which now includes the OpenCASCADE Community Edition).
This can be found at http://www.opencascade.org/getocc/download/loadocc or
https://github.com/tpaviot/oce. Prebuilt versions are available at these sites
for Windows using various versions of Visual Studio and for MAC OSX at 64-bits.
Any other configuration must be built from source (unless you are using a
Debian variant of LINUX, such as Ubuntu, where there are available prebuilt
packages as part of the LINUX distribution). Note that 6.5 is recommended but
there are SBO robustness problems at 6.5.3, so currently ESP works best with
6.5.2.

	Another prerequisite is a WebGL/Websocket capable Browser. In general
these include Mozilla's FireFox and Google Chrome. Apple's Safari works at
rev 6.0 or greater. Note that there is some problems with Intel Graphics and
some WebGL Browsers with Linux. Also, for LINUX "zlib" development is required.


2. Building the Software

	The config subdirectory contains scripts that need to be used to
generate the environment both to build and run the software here. There are 
two different procedures based on the OS:

2.1 Linux and Mac OSX

	The configuration is built using the path where the OpenCASCADE runtime
distribution can be found. The pointer size (32 or 64bit) is determined from 
the shared objects/dynamic libraries found in the distribution. This path can 
be located in an OpenCASCADE distribution by looking for a subdirectory that 
includes an "inc" or "include" directory and either a "lib" or "$ARCH/lib" 
(where $ARCH is the name of your architecture) directory.  For Debian prebuilt 
packaged installs this location is "/usr/include/opencascade".  Once that is 
found, execute the commands:

	% cd $DISTROOT/config
	% makeEnv **name_of_directory_containing_inc_and_lib**

An optional second argument to makeEnv is required if the distribution of 
OpenCASCADE has multiple architectures. In this case it is the subdirectory 
name that contains the libraries for the build of interest (CASARCH).

	This procedure produces 2 files at the top level: GEMenv.sh and
GEMenv.csh. These are the environments for both sh (bash) and csh (tcsh)
respectively. The appropriate file can be "source"d or included in the
user's startup scripts. This must be done before either building and/or
running the software.  For example, if using the csh or tcsh:

	% cd $DISTROOT
	% source GEMenv.csh

or if using bash:

	$ cd $DISTROOT
	$ source GEMenv.sh


2.2 Windows Configuration

	The configuration is built from the path where where the OpenCASCADE 
runtime distribution can be found. The pointer size (32 or 64bit) is determined
from the MS Visual Studio environment in a command shell (the C/C++ compiler
is run). This is executed simply by going to the config subdirectory and 
executing the script "winEnv" in a bash shell (run from the command window):

	C:\> cd $DISTROOT\config
	C:\> bash winEnv D:\OpenCASCADE6.5.2\ros

winEnv (like makeEnv) has an optional second argument that is only required 
if the distribution of OpenCASCADE has multiple architectures. In this case 
it is the subdirectory name that contains the libraries for the build of 
interest (CASARCH).

	This procedure produces a single file at the top level: GEMenv.bat.
This file needs to be executed before either building and/or running the 
software.  This is done with:

	C:\> cd $DISTROOT
	C:\> GEMenv

2.3 The Build

	For any of the operating systems, after properly setting the
environment in the command window (or shell), follow this simple procedure:

	% cd $DISTROOT/src
	% make

or

	C:\> cd $DISTROOT\src
	C:\> make

You can use "make clean" which will clean up all object modules or 
"make cleanall" to remove all objects, executables, libraries, shared objects
and dynamic libraries.


3.0 Running

3.1 vTess and wvClient

To start vTess there are two steps: (1) start the "server", in this case vTess
and (2) start the  "browser". This can be done in a variety of ways, but the 
two most common follow.  

3.1.1 Procedure 1: have wv automatically started from vTess

If it exists, the WV_START environment variable contains the command that 
should be executed to start the browser once the server has created its scene 
graph.  On a Mac, you can set this variable with commands such as

	% setenv WV_START "open -a /Applications/Firefox.app ../wvClient/wv.html"

or

	% export WV_START="open -a /Applications/Firefox.app ../wvClient/wv.html"

depending on the shell in use.  The commands in other operating systems will 
differ slightly, depending on how the browser can be started from the command 
line.

To run the program, use:

         % cd $DISTROOT/bin
         % vTess ../data/Piston.BRep

3.1.2 Procedure 2: start the browser manually

If the WV_START environment variable does not exist, issuing the commands:

	% cd $DISTROOT/bin
	% vTess ../data/Piston.BRep

will start the server.  The last lines of output from vTess tells the user 
that the server is waiting for a browser to attach to it.  This can be done by 
starting a browser (FireFox and Google Chrome have been tested) and loading the 
file:

        $DISTROOT/wvClient/wv.html

Whether you used procedure 1 or 2, as long as the browser stays connected to 
vTess, vTess will stay alive and handle requests sent to it from the browser. 
Once the last browser that is connected to vTess exits, vTess will shut down.

Once the browser starts, you will be prompted for a "hostname:port". Make the
appropriate response depending on the network situation.


3.2 egads2cart

	This example takes an input geometry file and generates a Cart3D "tri"
file. The acceptable input is STEP, EGADS or OpenCASCADE BRep files.

        % cd $DISTROOT/bin
	% egads2cart geomFilePath [angle relSide relSag]

C      Geometry Viewer (gv)
C
C             FORTRAN Include
C
C
C       Graphic data types
C
        INTEGER    GV_NONINDEXED,   GV_INDEXED
        PARAMETER (GV_NONINDEXED=0, GV_INDEXED=1)
C
        INTEGER    GV_POINTS,   GV_DISJOINTLINES,   GV_POLYLINES
        PARAMETER (GV_POINTS=0, GV_DISJOINTLINES=1, GV_POLYLINES=2)
        INTEGER    GV_DISJOINTTRIANGLES,   GV_POLYTRIANGLES
        PARAMETER (GV_DISJOINTTRIANGLES=3, GV_POLYTRIANGLES=4)
        INTEGER    GV_DISJOINTQUADS,   GV_QUADMESHS
        PARAMETER (GV_DISJOINTQUADS=5, GV_QUADMESHS=6)
C
C       Volume elements
C
        INTEGER    GV_TETRAHEDRA,   GV_PYRAMIDS,   GV_PRISMS
        PARAMETER (GV_TETRAHEDRA=8, GV_PYRAMIDS=9, GV_PRISMS=10)
        INTEGER    GV_HEXAHEDRA,    GV_POLYTETRAHEDRA,    GV_BLOCKS
        PARAMETER (GV_HEXAHEDRA=11, GV_POLYTETRAHEDRA=12, GV_BLOCKS=13)
C
C       Scalar types
C
        INTEGER    GV_POINT,   GV_LINE,   GV_LINEFACET
        PARAMETER (GV_POINT=0, GV_LINE=1, GV_LINEFACET=2)
        INTEGER    GV_SURF,   GV_SURFFACET,   GV_VOLU
        PARAMETER (GV_SURF=3, GV_SURFFACET=4, GV_VOLU=5)
        INTEGER    GV_VOLUFACET
        PARAMETER (GV_VOLUFACET=6)
C
C       Marker types
C
        INTEGER    GV_DOT,   GV_SPHERE,   GV_CROSSHAIR
        PARAMETER (GV_DOT=0, GV_SPHERE=1, GV_CROSSHAIR=2)
C
C       Plotting Atrributes (orrable)
C
        INTEGER    GV_OFF,   GV_SCALAR,   GV_FOREGROUND
        PARAMETER (GV_OFF=0, GV_SCALAR=1, GV_FOREGROUND=2)
C
        INTEGER    GV_ORIENTATION,   GV_TRANSPARENT,   GV_FACETLIGHT
        PARAMETER (GV_ORIENTATION=4, GV_TRANSPARENT=8, GV_FACETLIGHT=16)
C
        INTEGER    GV_MESH,    GV_FORWARD
        PARAMETER (GV_MESH=32, GV_FORWARD=64)
C
C
C       FUNCTIONAL INTERFACE:
C
C       INTEGER*4 FUNCTION FGV_INIT(title, mt_flag, nscalar, keys, types, 
C                                   lims, snames, focus)
C
C initializes GV
C
C C** title   - case title
C I*4 mt_flag - multi-trhead flag: -1 single process, dynamic definitions
C                                   0 single process, static graphics definitions
C                                   1 multi-threaded process, dynamic definitions
C I*4 nscalar - number of scalars used
C I*4 keys    - key activation for each scalar (nscalar in length)
C I*4 types   - scalar type (nscalar in len): 0 point
C                                             1 line (smooth shaded)
C                                             2 line
C                                             3 node (surface)
C                                             4 facet (surface)
C                                             5 node (volume element)
C                                             6 cell (volume element)
C R*8 lims    - min and max for color range (2*nscalar in len)
C C** snames  - title for each scalar (nscalar in len)
C R*8 focus   - [0] - [2]: focus of initial view transformation
C               [3] - radius of object
C
C returns        0 - OK
C         non zero - error
C
C
C       SUBROUTINE FGV_CLEANUP()
C
C frees all data associated with this interface
C
C
C
C       INTEGER*4 FUNCTION FGV_CREATE(gtype, attribute, fcolor, bcolor, title,
C                                     len, sizes, indices, ddata, utype, unidex)
C
C allocates, sets up and returns the graphics structure
C
C I*4 indexed   - 0 - object in ddata, 1 - indexed
C I*4 gtype     - the graphics type for first object
C I*4 attribute - the initial plotting attributes
C R*4 fcolor    - foreground color for object (3 in len - rgb)
C R*4 bcolor    - back-facing color (if any)
C C** title     - the name of the object
C I*4 len       - object length
C I*4 sizes     - sizes of graphic objects (if appropriate)
C I*4 indices   - indices (if indexed) 1 bias -- 0 non-indexed
C R*8 ddata     - coordinate data
C I*4 utype     - user type (0 - 31)
C I*4 uindex    - user index
C
C returns positive value - graphic object handle index
C           non positive - error
C
C
C       INTEGER*4 FUNCTION FGV_FREE(goh)
C
C frees up allocated graphic object
C
C I*4 goh - the graphics object handle to be freed
C
C returns  0 - OK
C
C
C       INTEGER*4 FUNCTION FGV_HANDSHAKE(hold, ngraphics)
C
C used for multi-threaded operation to allow the clean changing of data
C
C I*4 hold - hold/release flag: 1 hold the graphics thread from accessing the data
C                               0 release the graphics thread
C                              -1 kill the graphics thread
C                              -2 error call before exit
C I*4 ngraphics: 0 no data has changed
C                non-zero (positive) data has changed (only valid at release)
C                new number of graphics objects - forces a call to FGVDATA
C
C returns  1 - done
C          0 - OK
C         -1 - hold/release sync error
C
C
C       INTEGER*4 FUNCTION FGV_STATUS(ngraphics)
C
C return the current status
C
C I*4 ngraphics: returned number of active graphics
C
C returns    1 graphics thread has exited
C            0 running - no pending message strings
C           -n running with n pending (thrown) messages
C
C
C       INTEGER*4 FUNCTION FGV_LOCATE(xyz)
C
C return the picked 3D location - multi-threaded only
C  call waits for user to complete action
C
C R*8 xyz - the location
C
C returns  0 - OK
C         -1 - sync error
C         -2 - locate user aborted
C         -3 - graphics thread has exited (or is not active)
C         -4 - nothing selected
C
C
C       INTEGER*4 FUNCTION FGV_CATCH(string)
C
C returns the pointer to a string from a thrown message -  multi-threaded only
C  call waits for user to complete action - if messages are pending it
C  returns immediately
C
C C** string - the returned character string
C
C returns  0 - OK
C         -1 - graphics thread has exited (or is not active)
C         -2 - string is too short
C
C
C       INTEGER*4 FUNCTION FGV_ALLOCFAMILY(name)
C
C sets up a new family with the specified name
C
C C** name - family name
C
C returns  0 - OK
C         -1 - family already exists
C         -2 - allocate error
C
C
C       INTEGER*4 FUNCTION FGV_FREEFAMILY(name)
C
C deletes a family with the specified name and un-assigns all members
C
C C** name - family name
C
C returns  0 - OK
C         -1 - family does not exist
C
C
C       INTEGER*4 FUNCTION FGV_ADOPT(name, gohi)
C
C adds the graphic object to the specified family
C  if name is spaces then the graphic object becomes un-assigned
C
C C** name - family name
C I*4 gohi - the graphic object handle index
C
C returns  0 - OK
C         -1 - family does not exist
C         -2 - allocate error
C
C
C       INTEGER*4 FUNCTION FGV_SETFAMILY(name, type, attr)
C
C sets the plotting attribute for the family and type
C
C C** name - family name
C I*4 type =  0 -- point (input)
C             1 -- line
C             2 -- surface
C             3 -- volume element
C I*4 attr - plotting attribute
C
C returns  0 - OK
C         -1 - family does not exist
C         -2 - type error
C
C
C       INTEGER*4 FUNCTION FGV_GETFAMILY(name, type, attr)
C
C gets the plotting attribute for the family and type
C
C C** name - family name
C I*4 type =  0 -- point (input)
C             1 -- line
C             2 -- surface
C             3 -- volume element
C I*4 attr - returned plotting attribute
C
C returns  0 - OK
C         -1 - family does not exist
C         -2 - type error
C
C
C       INTEGER*4 FUNCTION FGV_NUMFAMILY()
C
C returns the number of active families
C
C
C       INTEGER*4 FUNCTION FGV_RETURNFAMILY(index, name)
C
C returns the family name based on 
C
C I*4 index - family index -- 1 to FGV_NUMFAMILY()
C C** name  - the returned name
C
C returns  0 - OK
C         -1 - index out of range
C         -2 - name length too short
C
C
C       SUBROUTINE FGV_DUMP(index, name)
C
C dumps the specified window index to a file for movie making (ppm or bmp)
C
C I*4 index - 2 or 3 for either 2D or 3D window
C C** name  - output filename
C
C
C
C       ROUTINES SUPPLIED:
C
C       INTEGER*4 FUNCTION FGVUPDATE()
C
C used for single process operation to allow the changing of data
C
C returns 0 no data has changed
C           non-zero (positive) data has changed
C           new number of graphics objects - forces a call to FGVDATA
C
C
C       SUBROUTINE FGVDATA(ngraphic, graphics)
C
C used to (re)set the graphics objects to be used in plotting
C   invoked by FGVUPDATE or FGV_HANDSHAKE
C
C I*4 ngraphic - is the number of objects to be filled by handle
C I*4 graphics - the vector of graphic object handles to view
C                filled to ngraphic in length
C
C
C       INTEGER*4 FUNCTION FGVSCALAR(key, gohi, len, scalar)
C
C called to get scalar value for color rendering for graphics objects
C
C I*4 key    - scalar index (from FGV_INIT)
C I*4 gohi   - the graphic object handle index
C I*4 len    - len of scalar to be filled
C R*4 scalar - scalar to be filled (length = len)
C
C returns 0 - no change (not filled)
C         1 - scalar filled
C
C
        INTEGER*4 FGV_INIT, FGV_CREATE, FGV_FREE, FGV_HANDSHAKE
        INTEGER*4 FGV_STATUS, FGV_LOCATE, FGV_CATCH, FGV_ALLOCFAMILY
        INTEGER*4 FGV_FREEFAMILY, FGV_ADOPT, FGV_SETFAMILY
        INTEGER*4 FGV_GETFAMILY, FGV_RETURNFAMILY


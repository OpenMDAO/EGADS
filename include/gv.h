/*
 *                   Geometry Viewer include
 *
 *         Copyright 1995 - 2011, Massachusetts Institute of Technology.
 *
 */

#ifndef _GV_H_

#define _GV_H_


/*
 *      Graphic data types
 */

#define GV_NONINDEXED        0
#define GV_INDEXED           1

#define GV_POINTS            0
#define GV_DISJOINTLINES     1
#define GV_POLYLINES         2
#define GV_DISJOINTTRIANGLES 3
#define GV_POLYTRIANGLES     4
#define GV_DISJOINTQUADS     5
#define GV_QUADMESHS         6

/*
 *      Volume elements
 */
 
#define GV_TETRAHEDRA        8
#define GV_PYRAMIDS          9
#define GV_PRISMS           10
#define GV_HEXAHEDRA        11
#define GV_POLYTETRAHEDRA   12
#define GV_BLOCKS           13

/*
 *      Scalar types
 */

#define GV_POINT             0
#define GV_LINE              1
#define GV_LINEFACET         2
#define GV_SURF              3
#define GV_SURFFACET         4
#define GV_VOLU              5
#define GV_VOLUFACET         6

/*
 *      Marker types
 */

#define GV_DOT               0
#define GV_SPHERE            1
#define GV_CROSSHAIR         2

/*
 *      Plotting Atrributes (orrable)
 */
 
#define GV_OFF               0

#define GV_SCALAR            1        /* not with FOREGROUND */
#define GV_FOREGROUND        2        /* not with SCALAR */

#define GV_ORIENTATION       4
#define GV_TRANSPARENT       8
#define GV_FACETLIGHT       16        /* not set -> smooth lighting */

#define GV_MESH             32        /* plot mesh upon surfaces */
#define GV_FORWARD          64        /* move lines forward */


/*
 *      Structures
 */

typedef struct {
  float red;                    /* red   intensity (0.0 - 1.0) */
  float green;                  /* green intensity (0.0 - 1.0) */
  float blue;                   /* blue  intensity (0.0 - 1.0) */
} GvColor;

typedef struct {
  char  name[16];               /* scalar name */
  int   ikey;                   /* X-keysym for scalar */
  int   type;                   /* scalar type */
  float kmin;                   /* minimum for key */
  float kmax;                   /* maximum for key */
} GvKey;

typedef struct {
  int     key;                  /* index for the GvKey */
  int     cmaplen;              /* length of color map (3*cmaplen floats) */
  float   *cmap;                /* color map */
  GvColor bg;                   /* background color */
  float   min;                  /* current minimum for key */
  float   max;                  /* current maximum for key */
} GvRamp;

typedef struct {
  int   marker;                 /* marker type */
  float size;                   /* size of plotted representation */
  int   *index;                 /* indices for indexed data */
  int   pick;                   /* index into data for pick */
} GvPoints;

typedef struct {
  float size;                   /* size of plotted attribute */
                                /* length of data must be 2*length */
  int   *index;                 /* indices for indexed data */
  int   orientation;            /* 0 - as is; 1 - flip */
  int   pick;                   /* index into data for pick - the first */
} GvDJLines;

typedef struct {
  float size;                   /* size of plotted attribute */
  int   *len;                   /* length of each polyline */
                                /* length of data must be the sum of len */
  int   *index;                 /* indices for indexed data */
  int   orientation;            /* 0 - as is; 1 - flip */
  int   pick[2];                /* polyline #, segment # (first is 0) */
} GvPLines;

typedef struct {
  int   *index;                 /* index triads into data 
                                   length of index must be 3*length */
  int   orientation;            /* 0 - as is; 1 - flip */
  int   pick;                   /* Triangle # picked (first is 0) */
  float *facetnormals;          /* facet normals (local) -NULL=not calculated */
} GvDJTris;

typedef struct {
  int   *len;                   /* length of each polytri list */
                                /* length of data must be sum of len */
  int   *index;                 /* indices for indexed data */
  int   orientation;            /* 0 - as is; 1 - flip */
  int   pick[2];                /* # of polytri, triangle picked (first is 0) */
  float *facetnormals;          /* facet normals (local) -NULL=not calculated */
} GvPTris;

typedef struct {
  int   *index;                 /* indices (4 per quad) into data 
                                   length of index must be 4*length */
  int   orientation;            /* 0 - as is; 1 - flip */
  int   pick;                   /* Quad # picked (first is 0) */
  float *facetnormals;          /* facet normals (local) -NULL=not calculated */
} GvDJQuads;

typedef struct {
  int   *size;                  /* size (nu, nv) - length of size = 2*length */
                                /*  length of data is sum of sizes (nu*nv) */
  int   *index;                 /* indices for indexed data */
  int   orientation;            /* 0 - as is; 1 - flip */
  int   pick[3];                /* mesh #, iu, iv */
  float *facetnormals;          /* facet normals (local) -NULL=not calculated */
} GvQMeshes;

typedef struct {
  int   *index;                 /* indices (4 per tet) into data */
  int   pick;                   /* Tetrahedra picked - first is zero */
} GvTets;

typedef struct {
  int   *index;                 /* indices (5 per pyramid) into data */
  int   pick;                   /* Pyramid picked - first is zero */
} GvPyramids;

typedef struct {
  int   *index;                 /* indices (6 per prism) into data */
  int   pick;                   /* Prism picked - first is zero */
} GvPrisms;

typedef struct {
  int   *index;                 /* indices (8 per hexa) into data */
  int   pick;                   /* Hexa picked - first is zero */
} GvHexas;

typedef struct {
  int   *len;                   /* length of each polytetral list */
                                /* length is the sum of the data in len */
  int   *index;                 /* indices for indexed data */
  int   pick[2];                /* # of polytet, tet within polytet (0 is first) */
} GvPTets;

typedef struct {
  int   *size;                  /* size (nx, ny, nz) - length of size = 3*length */
                                /* length of data is the sum of sizes (nx*ny*nz) */
  int   pick[4];                /* mesh #, ix, iy, iz */
} GvBlocks;

typedef struct {
  int gtype;                    /* graphics type */
  int length;                   /* length of object */
  union {
    GvPoints   points;          /* length is the number of points */
    GvDJLines  dislines;        /* length is the number of line segments */
    GvPLines   plines;          /* length is the number of polylines */
    GvDJTris   distris;         /* length is the number of disjoint tris */
    GvPTris    ptris;           /* length is the number of polytris */
    GvDJQuads  disquads;        /* length is the number of disjoint quads */
    GvQMeshes  qmeshes;         /* length is the number of meshes */
    GvTets     tets;            /* length is the number of tets */
    GvPyramids pyramids;        /* length is the number of pyramids */
    GvPrisms   prisms;          /* length is the number of prisms */
    GvHexas    hexas;           /* length is the number of hexas */
    GvPTets    ptets;           /* length is the number of polytet lists */
    GvBlocks   blocks;          /* length is the number of blocks */
  } type;
} GvObject;

typedef struct GvFamily {
  char   *name;			/* family name */
  int    fexposed;		/* contents exposed */
  int    fdbstart;		/* db index for start */
  int    nentities[4];		/* number of entities for type */
  int    attributes[4];		/* plot attributes */
  int    exposed[4];		/* entitity contents exposed */
  int    dbstart[4];		/* db index for start */
  struct GvFamily *next;	/* pointer to next family */
} GvFamily;

typedef struct GvGraphic {
  GvFamily *family;             /* pointer to family - NULL not assigned */
  int      attribute;           /* plotting attribute mask */
  int      indexed;             /* 0 - object in f|ddata, 1 - indexed */
  GvColor  color;               /* (foreface) color of object */
  GvColor  back;                /* color of back faces (surfaces only) */
  GvColor  mesh;                /* mesh color (surfaces/vol only) */
  float    lineWidth;           /* line width in pixels */
  float    pointSize;           /* point size in pixels */
  char     title[16];           /* graphics title */
  int      utype;               /* user specified type (0-31) */
  int      uindex;              /* user index */
  int      number;              /* number of objects */
  GvObject *object;             /* object */
  int      datalen;             /* length of ddata and/or fdata */
  double   *ddata;              /* pointer to coordinates (double if used) */
  float    *fdata;              /* pointer to coordinates - filled by gv
                                   if double is used */
  float    *nodenormals;        /* node normals (local) - NULL=not calculated */
  int      pickindex;           /* index for picked object */
  struct   GvGraphic *next;     /* pointer to next graphic */
} GvGraphic;

/*
 * composite objects in the same graphic must be indexed and of similar type.
 *   the objects use the same "node space" (fdata or ddata), i.e. share the
 *   coordinates (and contribute to the node-normal calculation, if surfaces).
 *
 *   lines:           GvDJLines, GvPLines
 *   surfaces:        GvDJTris,  GvPTris,    GvDJQuads, GvQMeshes
 *   volume elements: GvTets,    GvPyramids, GvPrisms,  GvHexas,  GvPTets
 */


#ifdef __ProtoGlarp__
#undef __ProtoGlarp__
#endif
#if defined(__STDC__) || defined(__cplusplus) || defined(WINNT)
#define __ProtoGlarp__(x) x
#else
#define __ProtoGlarp__(x) ()
#endif

#ifdef __cplusplus
extern "C" {
#endif

int gv_init             __ProtoGlarp__(( char *title, int mt_flag, int nscalar, 
                                         int *keys, int *types, float *lims, 
                                         char *titles, float *focus ));
/*
 *  title   - case title
 *  mt_flag - multi-trhead flag: -1 single process, dynamic definitions
 *                                0 single process, static graphics definitions
 *                                1 multi-threaded process, dynamic definitions
 *  nscalar - number of scalars used
 *  keys    - key activation for each scalar
 *  types   - scalar type: 0 point
 *                         1 line (smooth shaded)
 *                         2 line
 *                         3 node (surface)
 *                         4 facet (surface)
 *                         5 node (volume element)
 *                         6 cell (volume element)
 *  lims    - min and max for color range
 *  titles  - title for each scalar (16 characters)
 *  focus   - [0] - [2]: focus of initial view transformation
 *            [3] - radius of object
 *
 *  returns        0 - OK
 *          non zero - error
 */
 

/*@null@*/
GvGraphic *gv_alloc     __ProtoGlarp__(( int indexed, int gtype, int attribute,
                                         GvColor color, char *title, int utype,
                                         int uindex ));
/*
 *  allocates, sets up and returns the graphics structure
 *
 *  indexed   - 0 - object in f|ddata, 1 - indexed
 *  gtype     - the graphics type for first object
 *  attribute - the initial plotting attributes
 *  color     - foreground color for object
 *  title     - the name of the object
 *  utype     - user type (0 - 31)
 *  uindex    - user index
 */


int gv_addobject        __ProtoGlarp__((  GvGraphic* graphic, int gtype  ));

/*
 *  adds another object to the graphics structure
 *
 *  gtype     - the graphics type to be added
 *
 *  returns  0 - OK
 *          -1 - new object incompatible with graphic
 *          -2 - alloc error
 *          -3 - illegal gtype or not-indexed
 */


void gv_touchgraphic    __ProtoGlarp__((  GvGraphic* graphic  ));

/*
 *  removes local structures after graphic has changed
 *
 *  NOTE: if the change involves going from double to float:
 *           free and set graphic->ddata to NULL
 */

                                          
void gv_free            __ProtoGlarp__((  GvGraphic* graphic, int all ));

/*
 *  frees up all local storage and deallocates the structure
 *  all: 0 - only free up local (normals) storage
 *       1 - free up all non-NULL pointers except fdata/ddata on indexed types
 *       2 - free up all non-NULL pointers
 */


int gv_handshake        __ProtoGlarp__(( int hold, int ngraphics ));

/*
 *  used for multi-threaded operation to allow the clean changing of data
 *
 *  hold - hold/release flag: 1 hold the graphics thread from accessing the data
 *                            0 release the graphics thread
 *                           -1 kill the graphics thread
 *                           -2 error call before exit
 *  ngraphics: 0 no data has changed
 *             non-zero (positive) data has changed (only valid at release)
 *             new number of graphics objects - forces a call to gvgraphics
 *
 *  returns  1 - done
 *           0 - OK
 *          -1 - hold/release sync error
 */


int gv_status           __ProtoGlarp__(( int *ngraphics ));

/*
 *  return the current status
 *
 *  ngraphics: current number of graphics objects
 *
 *  returns    1 graphics thread has exited
 *             0 running - no pending message strings
 *            -n running with n pending (thrown) messages
 */


int gv_pick             __ProtoGlarp__(( int typemask, GvGraphic **ret ));

/*
 *  return the closest picked object - multi-threaded only
 *    call waits for user to complete action
 *
 *  typemask - orrable mask of the valid current user types to pick from
 *  ret      - the selected Graphics structure with the pick field filled
 *             NULL is an indication that nothing was picked
 *
 *  returns  0 - OK
 *          -1 - sync error
 *          -2 - pick user aborted
 *          -3 - graphics thread has exited (or is not active)
 *          -4 - zero typemask (or no current matches)
 */


int gv_locate           __ProtoGlarp__(( double *ret ));

/*
 *  return the picked 3D location - multi-threaded only
 *    call waits for user to complete action
 *
 *  returns  0 - OK
 *          -1 - sync error
 *          -2 - locate user aborted
 *          -3 - graphics thread has exited (or is not active)
 *          -4 - nothing selected
 */


int gv_catch		__ProtoGlarp__(( char **ret ));

/*
 *  returns the pointer to a string from a thrown message -  multi-threaded only
 *    call waits for user to complete action - if messages are pending it
 *    returns immediately
 *  this string should be freed by the caller
 *
 *  returns  0 - OK
 *          -1 - graphics thread has exited (or is not active)
 *
 */


int gv_allocfamily      __ProtoGlarp__(( char *name ));

/*
 *  sets up a new family with the specified name
 *
 *  returns  0 - OK
 *          -1 - family already exists
 *          -2 - allocate error
 *
 */


int gv_freefamily       __ProtoGlarp__(( char *name ));

/*
 *  deletes a family with the specified name and un-assigns all members
 *
 *  returns  0 - OK
 *          -1 - family does not exist
 *
 */


int gv_adopt            __ProtoGlarp__(( char *name, GvGraphic* graphic ));

/*
 *  adds the graphic to the specified family
 *    if *name is NULL the graphics becomes un-assigned
 *
 *  returns  0 - OK
 *          -1 - family does not exist
 *          -2 - allocate error
 *
 */


int gv_setfamily       __ProtoGlarp__(( char *name, int type, int attr ));

/*
 *  sets the plotting attribute for the family
 *    type = 0 -- point
 *           1 -- line
 *           2 -- surface
 *           3 -- volume element
 *
 *  returns  0 - OK
 *          -1 - family does not exist
 *          -2 - type error
 *
 */


int gv_getfamily       __ProtoGlarp__(( char *name, int type, int *attr ));

/*
 *  gets the plotting attribute for the family
 *    type = 0 -- point
 *           1 -- line
 *           2 -- surface
 *           3 -- volume element
 *
 *  returns  0 - OK
 *          -1 - family does not exist
 *          -2 - type error
 *
 */


int gv_numfamily       __ProtoGlarp__(( void ));

/*
 *   returns the number of families
 *
 */


int gv_returnfamily    __ProtoGlarp__(( int index, char **name ));

/*
 *   returns the family name based on index -- 0 to gv_numfamily()-1
 *
 *   returns  0 - OK
 *           -1 - index out of range
 *
 */


void gv_dump		__ProtoGlarp__(( int index, char *name ));

/*
 *  dumps the specified window index (either 2 or 3) to a file
 *        for movie making (ppm or bmp)
 *
 */


#ifdef __cplusplus
}
#endif

#endif  /*_GV_H_*/

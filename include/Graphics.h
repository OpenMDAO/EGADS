/*
 *	Geometry Viewer Graphics Include
 *
 *	Copyright 1995 - 2011, Massachusetts Institute of Technology.
 */

#ifndef _GRAPHICS_H_

#define _GRAPHICS_H_


/*  Graphics Defines */

#define GID   long

#define GNONE -1
#define GOFF  0
#define GON   1

#define KeyPress       2
#define KeyRelease     3
#define ButtonPress    4
#define ButtonRelease  5
#define Expose        12
#define NoExpose      14
#define ClientMessage 33

#define Button1	       1
#define Button2	       2
#define Button3	       3


/*
 *      Structures
 */

typedef struct {
  GID wid;                            /* window ID */
  int xsize;                          /* number of pixels in x */
  int ysize;                          /* number of pixels in y */
  GID GCs;                            /* GC with a small font */
  GID GCb;                            /* GC with a big font */
} GWinAtt;


/* normal mask: 0     - one normal for all nodes/facits
                1 & 3 - node normals
                2     - facit normals
                4     - one normal for all nodes/facits and flip orientation
                5 & 7 - node normals and flip orientation
                6     - facit normals and flip orientation
*/


/* Graphics Routines */

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

void	GraphicInit __ProtoGlarp__(( int *rootx, int *rooty, int *matte ));
GID *	GraphicEventStructure __ProtoGlarp__(( void ));
GID	GraphicWindow __ProtoGlarp__(( char *string, int xsize, int ysize,
	                               int xstart, int ystart, int resize ));
GID	GraphicGC __ProtoGlarp__(( GID window, int size ));
void	GraphicFreeGC __ProtoGlarp__(( GID gc ));
void	GraphicWindowStatus __ProtoGlarp__(( GID window, int *xsize, int *ysize,
	                                     int *xstart, int *ystart ));
void	GraphicClearWindow __ProtoGlarp__(( GID window ));
int	GraphicNumEvents __ProtoGlarp__(( void ));
GID	GraphicEvent __ProtoGlarp__(( int *type, int *x, int *y, int *state ));
int	GraphicGetEvent __ProtoGlarp__(( GID window, int type, int *x, int *y,
	                                 int *state ));
void	GraphicLowerWindow __ProtoGlarp__(( GID window ));
void	GraphicText __ProtoGlarp__(( GID window, GID gc, int x, int y,
	                             char   *text ));
void	GraphicLine __ProtoGlarp__(( GID window, GID gc, int x1, int y1,
	                             int x2, int y2 ));
void	GraphicArc __ProtoGlarp__(( GID window, GID gc, int xc, int yc,
	                            int xr, int yr, float alpha, float beta ));
void	GraphicFillArc __ProtoGlarp__(( GID window, GID gc, int xc, int yc,
	                                int xr, int yr, float alpha,
                                        float beta ));
void	GraphicPointer __ProtoGlarp__(( GID window, int *x, int *y,
                                        unsigned int *key_buttons ));
void	GraphicBGColor __ProtoGlarp__(( GID window, float *color ));
void	GraphicGCSetFB __ProtoGlarp__(( GID gc, float *fg, float *bg ));
void	GraphicColorCursor __ProtoGlarp__(( int type ));
void	GraphicRubber __ProtoGlarp__(( GID window, int *type, int *x1, int *y1,
	                               int *x2, int *y2, int *state ));
void	GraphicSynchronize __ProtoGlarp__(( int onoff ));
void	GraphicGCColor __ProtoGlarp__(( GID gc, float *color ));
void	GraphicFlush __ProtoGlarp__(( void ));
void	GraphicDestroyWindow __ProtoGlarp__(( GID window ));
void	GraphicCloseDisplay __ProtoGlarp__(( void ));
GID	GraphicCurrentPointer __ProtoGlarp__(( int *x, int *y ));
void	GraphicSetPointer __ProtoGlarp__(( GID window, int x, int y ));
void	GraphicDump __ProtoGlarp__(( GID window, char *title, char *title3,
	                             char *filename));
void	GraphicFrame __ProtoGlarp__(( GID window, char *filename));
void	GraphicLogo __ProtoGlarp__(( GID drawable, GID gc, int *x, int *y ));

void	GraphicReconnect __ProtoGlarp__(( GID window ));
void	GraphicGLWait __ProtoGlarp__(( GID window ));
void	GraphicSetFont __ProtoGlarp__(( GID window, int size ));
void	GraphicPointSize __ProtoGlarp__(( GID window, float size ));
void	GraphicLineWidth __ProtoGlarp__(( GID window, float width ));
void	GraphicView __ProtoGlarp__(( GID window, float *matx ));
void	GraphicLighting __ProtoGlarp__(( GID window, int flag ));
void	GraphicLight __ProtoGlarp__(( float zr, float *la, int nl, float *lv,
	                              float *lc ));
void	GraphicStart __ProtoGlarp__(( GID window, float *color, int type ));
void	GraphicEnd __ProtoGlarp__(( GID window ));
void	GraphicMaterial __ProtoGlarp__(( GID window, int type, float *bcol ));
void	GraphicGLText __ProtoGlarp__(( GID window, float *pos, float *color,
	                               char *text ));
void	GraphicSetXparency __ProtoGlarp__(( GID window, float xpar ));
void	GraphicDepthQ __ProtoGlarp__(( GID window, float *color ));
int	GraphicRendererID  __ProtoGlarp__(( void ));
GID	GraphicGLWindow __ProtoGlarp__(( char *string, int w, int h,
	                                 int x, int y, int resize ));
void	GraphicKey __ProtoGlarp__(( GID window, float *bg , int ncol,
                                    float *cols));
void    GraphicGLdraw __ProtoGlarp__(( GID window, int onoff ));

void	GraphicPrim2D __ProtoGlarp__(( GID window, float *lcol, float *vert,
                                       int n ));
void	GraphicPrim __ProtoGlarp__(( GID window, float *lcol, float *vert,
                                     float *norms, int n, int flip ));
void	GraphicPoint2D __ProtoGlarp__(( GID window, float *lcol, float *vert,
	                                int n ));
void	GraphicPointColor2D __ProtoGlarp__(( GID window, float *cmap, int *ci,
	                                     float *vert, int n ));
void	GraphicPoint __ProtoGlarp__(( GID window, float *lcol, float *vert,
	                              int n ));
void	GraphicPointColor __ProtoGlarp__(( GID window, float *cmap, int *ci,
	                                   float *vert, int n ));
void	GraphicIPoint2D __ProtoGlarp__(( GID window, float *lcol, float *vert,
	                                 int *index, int n ));
void	GraphicIPointColor2D __ProtoGlarp__(( GID window, float *cmap, int *ci,
	                                      float *vert, int *index, int n ));
void	GraphicIPoint __ProtoGlarp__(( GID window, float *lcol, float *vert,
	                               int *index, int n ));
void	GraphicIPointColor __ProtoGlarp__(( GID window, float *cmap, int *ci,
	                                    float *vert, int *index, int n ));
void	GraphicArrowHead __ProtoGlarp__(( GID window, float *lcol, float *hpt,
                                          float *tpt, float size, float sprd ));
void	GraphicPolyLine2D __ProtoGlarp__(( GID window, float *lcol,
	                                   float *vert, int n ));
void	GraphicPolyLineColor2D __ProtoGlarp__(( GID window, float *cmap,
	                                        int *ci, float *vert, int n ));
void	GraphicPolyLine __ProtoGlarp__(( GID window, float *lcol,
	                                 float *vert, int n, int mirror ));
void	GraphicPolyLineColor __ProtoGlarp__(( GID window, float *cmap, int *ci,
	                                      float *vert, int n, int mirror ));
void	GraphicIPolyLine2D __ProtoGlarp__(( GID window, float *lcol,
	                                   float *vert, int *index, int n ));
void	GraphicIPolyLineColor2D __ProtoGlarp__(( GID window, float *cmap,
	                                        int *ci, float *vert, int *index,
	                                        int n ));
void	GraphicIPolyLine __ProtoGlarp__(( GID window, float *lcol, float *vert,
                                          int *index, int n, int mirror ));
void	GraphicIPolyLineColor __ProtoGlarp__(( GID window, float *cmap, int *ci,
	                                      float *vert, int *index, int n,
	                                      int mirror ));
void	GraphicDisjointLineColor2D __ProtoGlarp__(( GID window, float *cmap,
	                                            int *ci, float *vert,
	                                            int n ));
void	GraphicDisjointLine2D __ProtoGlarp__(( GID window, float *lcol,
	                                       float *vert, int n ));
void	GraphicDisjointLineColor __ProtoGlarp__(( GID window, float *cmap,
	                                          int *ci, float *vert, int n,
	                                          int mirror ));
void	GraphicDisjointLine __ProtoGlarp__(( GID window, float *lcol,
	                                     float *vert, int n, int mirror ));
void	GraphicIDisjointLineColor2D __ProtoGlarp__(( GID window, float *cmap,
	                                             int *ci, float *vert,
	                                             int *index, int n ));
void	GraphicIDisjointLine2D __ProtoGlarp__(( GID window, float *lcol,
	                                        float *vert, int *index, int n ));
void	GraphicIDisjointLineColor __ProtoGlarp__(( GID window, float *cmap,
	                                          int *ci, float *vert, int *index,
	                                          int n, int mirror ));
void	GraphicIDisjointLine __ProtoGlarp__(( GID window, float *lcol, float *vert,
                                              int *index, int n, int mirror ));
void	GraphicDisjointTriangleColor2D __ProtoGlarp__(( GID window, float *cmap,
	                                                int *ci, float *vert,
	                                                int n ));
void	GraphicDisjointTriangle2D __ProtoGlarp__(( GID window, float *lcol,
	                                           float *vert, int n ));
void	GraphicDisjointTriangleColor __ProtoGlarp__(( GID window, float *cmap,
	                                              float *norms, int *ci,
	                                              float *vert, int n,
	                                              int mirror, int nf ));
void	GraphicDisjointTriangle __ProtoGlarp__(( GID window, float *lcol,
	                                         float *norms, float *vert,
	                                         int n, int mirror, int nf ));
void	GraphicDisjointTriangleMesh __ProtoGlarp__(( GID window, float *lcol, 
                                                     float *vert, int n, 
                                                     int mirror ));
void	GraphicIDisjointTriangleColor2D __ProtoGlarp__(( GID window, float *cmap,
	                                                 int *ci, float *vert,
	                                                 int *index, int n ));
void	GraphicIDisjointTriangle2D __ProtoGlarp__(( GID window, float *lcol,
	                                            float *vert, int *index, 
	                                            int n ));
void	GraphicIDisjointTriangleColor __ProtoGlarp__(( GID window, float *cmap,
	                                               float *norms, int *ci,
	                                               float *vert, int *index,
	                                               int n, int mirror, int nf ));
void	GraphicIDisjointTriangle __ProtoGlarp__(( GID window, float *lcol,
	                                          float *norms, float *vert,
	                                          int *index, int n, int mirror,
	                                          int nf ));
void	GraphicIDisjointTriangleMesh __ProtoGlarp__(( GID window, float *lcol, 
	                                              float *vert, int *index, 
	                                              int n, int mirror ));
void	GraphicDisjointQuadColor2D __ProtoGlarp__(( GID window, float *cmap,
	                                            int *ci, float *vert,
	                                            int n ));
void	GraphicDisjointQuad2D __ProtoGlarp__(( GID window, float *lcol,
	                                       float *vert, int n ));
void	GraphicDisjointQuadColor __ProtoGlarp__(( GID window, float *cmap,
	                                          float *norms, int *ci,
	                                          float *vert, int n,
	                                          int mirror, int nf ));
void	GraphicDisjointQuad __ProtoGlarp__(( GID window, float *lcol,
	                                     float *norms, float *vert,
	                                     int n, int mirror, int nf ));
void	GraphicDisjointQuadMesh __ProtoGlarp__(( GID window, float *lcol, 
	                                         float *vert, int n, int mirror ));
void	GraphicIDisjointQuadColor2D __ProtoGlarp__(( GID window, float *cmap,
	                                             int *ci, float *vert,
	                                             int *index, int n ));
void	GraphicIDisjointQuad2D __ProtoGlarp__(( GID window, float *lcol,
	                                        float *vert, int *index, int n ));
void	GraphicIDisjointQuadColor __ProtoGlarp__(( GID window, float *cmap,
	                                           float *norms, int *ci,
	                                           float *vert, int *index, int n,
	                                           int mirror, int nf ));
void	GraphicIDisjointQuad __ProtoGlarp__(( GID window, float *lcol,
	                                      float *norms, float *vert, 
	                                      int *index, int n, int mirror,
	                                      int nf ));
void	GraphicIDisjointQuadMesh __ProtoGlarp__(( GID window, float *lcol, 
	                                          float *vert,int *index, 
	                                          int n, int mirror ));
void	GraphicPolyTriangle2D __ProtoGlarp__(( GID window, float *lcol,
	                                       float *vert, int n ));
void	GraphicPolyTriangleColor2D __ProtoGlarp__(( GID window, float *cmap,
	                                            int *ci, float *vert,
	                                            int n ));
void	GraphicPolyTriangle __ProtoGlarp__(( GID window, float *lcol,
	                                     float *norms, float *vert,
	                                     int n, int mirror, int nf ));
void	GraphicPolyTriangleColor __ProtoGlarp__(( GID window, float *cmap,
	                                          float *norms, int *ci,
	                                          float *vert, int n,
	                                          int mirror, int nf ));
void	GraphicPolyTriangleMesh __ProtoGlarp__(( GID window, float *lcol, 
                                                 float *vert, int n, int mir ));
void	GraphicIPolyTriangle2D __ProtoGlarp__(( GID window, float *lcol,
	                                        float *vert, int *index, int n ));
void	GraphicIPolyTriangleColor2D __ProtoGlarp__(( GID window, float *cmap,
	                                             int *ci, float *vert,
	                                             int *index, int n ));
void	GraphicIPolyTriangle __ProtoGlarp__(( GID window, float *lcol,
	                                      float *norms, float *vert,
	                                      int *index, int n, int mirror,
	                                      int nf ));
void	GraphicIPolyTriangleColor __ProtoGlarp__(( GID window, float *cmap,
	                                          float *norms, int *ci,
	                                          float *vert, int *index, 
	                                          int n, int mirror, int nf ));
void	GraphicIPolyTriangleMesh __ProtoGlarp__(( GID window, float *lcol, 
                                                  float *vert, int *index, 
                                                  int n, int mirror ));
void	GraphicQuadMeshColor2D __ProtoGlarp__(( GID window, float *cmap,
	                                        int *ci, float *vert,
	                                        int nx, int ny ));
void	GraphicQuadMesh2D __ProtoGlarp__(( GID window, float *lcol,
	                                   float *vert, int nx, int ny ));
void	GraphicQuadMeshColor __ProtoGlarp__(( GID window, float *cmap,
	                                      float *norms, int *ci,
	                                      float *vert, int nx, int ny,
	                                      int mirror, int nf ));
void	GraphicQuadMesh __ProtoGlarp__(( GID window, float *lcol,
	                                 float *norms, float *vert,
	                                 int nx, int ny, int mirror, int nf ));
void	GraphicQuadMeshMesh __ProtoGlarp__(( GID window, float *lcol, float *vert, 
	                                     int nx, int ny, int mirror ));
void	GraphicIQuadMeshColor2D __ProtoGlarp__(( GID window, float *cmap,
	                                         int *ci, float *vert, 
                                                 int *index, int nx, int ny ));
void	GraphicIQuadMesh2D __ProtoGlarp__(( GID window, float *lcol,
	                                   float *vert, int *index, 
                                           int nx, int ny ));
void	GraphicIQuadMeshColor __ProtoGlarp__(( GID window, float *cmap,
	                                       float *norms, int *ci, 
                                               float *vert, int *index, int nx,
                                               int ny, int mirror, int nf ));
void	GraphicIQuadMesh __ProtoGlarp__(( GID window, float *lcol,
	                                  float *norms, float *vert, int *index,
	                                  int nx, int ny, int mirror, int nf ));
void	GraphicIQuadMeshMesh __ProtoGlarp__(( GID window, float *lcol,
                                              float *vert, int *index,
	                                      int nx, int ny, int mirror ));

int	GraphicInitDial __ProtoGlarp__(( void ));
void	GraphicSetDials __ProtoGlarp__(( GID win, int x, int y, int active ));
int	GraphicGetDials __ProtoGlarp__(( int *rev ));

#ifdef __cplusplus
}
#endif

#endif  /*_GRAPHICS_H_*/

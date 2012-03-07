/*
 *      EGADS: Electronic Geometry Aircraft Design System
 *
 *             Make a Solid Wing from a single surface WireFrame w/ Viz
 *
 *      Copyright 2011-2012, Massachusetts Institute of Technology
 *      Licensed under The GNU Lesser General Public License, version 2.1
 *      See http://www.opensource.org/licenses/lgpl-2.1.php
 *
 */

#include <stdio.h>
#include <math.h>
#include <string.h>

#include "egads.h"
#include "gv.h"


  static int    sizes[2], sizei[2];
  static double *xyzs,    *xyzi;
  
  
int main(int argc, char *argv[])
{
  int    i, lx, nx, ny, oclass, mtype, per, stat, mtflag = 0, *ivec;
  int    nl, ne, nc, nn, *senses, sense;
  float  focus[4], box[6], arg;
  char   *str, line[81];
  double tol, limits[4], results[18], *rvec;
  ego    context, geom, tess, ref, face, *loops, *edges, *nodes;
  ego    cloop[2], cap[3], shell, solid, model;
  FILE   *fp;

  if (argc != 3) {
    printf("\n Usage: vWireWing filename tol\n\n");
    return 1;
  }

  fp = fopen(argv[1], "r");
  if (fp == NULL) {
    printf("\n filename %s Not Found!\n\n", argv[1]);
    return 1;
  }

  lx = nx = ny = 0;
  do {
    str = fgets(line, 80, fp);
    if (str == NULL) break;
    for (i = 0; i < 80; i++) {
      if (line[i] ==   0) break;
      if (line[i] ==  10) break;
      if (line[i] != ' ') break;
    }
    if ((i == 80) || (line[i] == 0) || (line[i] == 10)) {
      if (lx != 0) 
        if (lx != nx) {
          printf(" Mismatch number of points %d vs %d\n", lx, nx);
          return 1;
        }
      lx = nx;
      nx = 0;
      ny++;
    } else {
      nx++;
    }
  } while (str != NULL);
  sizei[0] = nx = lx;
  sizei[1] = ny;
  printf(" nx, ny = %d %d\n", nx, ny);
  xyzi = (double *) malloc(nx*ny*3*sizeof(double));
  if (xyzi == NULL) {
    printf(" MALLOC Error!\n\n");
    return 1;
  }
  rewind(fp);
  for (i = 0; i < nx*ny; i++) {
    fscanf(fp, "%lf %lf %lf", &xyzi[3*i  ], &xyzi[3*i+1], &xyzi[3*i+2]);
    if (i == 0) {
      box[0] = xyzi[3*i  ];
      box[1] = xyzi[3*i+1];
      box[2] = xyzi[3*i+2];
      box[3] = xyzi[3*i  ];
      box[4] = xyzi[3*i+1];
      box[5] = xyzi[3*i+2];
    } else {
      if (xyzi[3*i  ] < box[0]) box[0] = xyzi[3*i  ];
      if (xyzi[3*i+1] < box[1]) box[1] = xyzi[3*i+1];
      if (xyzi[3*i+2] < box[2]) box[2] = xyzi[3*i+2];
      if (xyzi[3*i  ] > box[3]) box[3] = xyzi[3*i  ];
      if (xyzi[3*i+1] > box[4]) box[4] = xyzi[3*i+1];
      if (xyzi[3*i+2] > box[5]) box[5] = xyzi[3*i+2];
    }
  }
  fclose(fp);
  i--;
  printf(" Last data = %lf %lf %lf\n", xyzi[3*i  ], xyzi[3*i+1], xyzi[3*i+2]);

  sscanf(argv[2], "%f", &arg);
  tol = arg;
  printf(" Tolerance = %lf\n", tol);

  /* initialize */
  printf(" EG_open           = %d\n", EG_open(&context));

  stat = EG_approximate(context, 3, tol, sizei, xyzi, &geom);
  if (stat != EGADS_SUCCESS) {
    printf(" EG_approximate return = %d!\n", stat);
    goto plot;
  }
  stat = EG_getGeometry(geom, &oclass, &mtype, &ref, &ivec, &rvec);
  if (stat != EGADS_SUCCESS) {
    printf(" EG_getGeometry return = %d\n", stat);
    return 1;
  }
  printf(" flags = %x, U deg = %d #CPs = %d #knots = %d ",
         ivec[0], ivec[1], ivec[2], ivec[3]);
  printf(" V deg = %d #CPs = %d #knots = %d\n",
         ivec[4], ivec[5], ivec[6]);
  EG_free(ivec);
  EG_free(rvec);

  stat = EG_getRange(geom, limits, &per);
  if (stat != EGADS_SUCCESS) {
    printf(" EG_getRange return = %d\n", stat);
    return 1;
  }
  sizes[0] =  2*sizei[0];
  sizes[1] = 16*sizei[1];
  stat = EG_makeTessGeom(geom, limits, sizes, &tess);
  if (stat != EGADS_SUCCESS) {
    printf(" EG_makeTessGeom Face return = %d!\n", stat);
    return 1;
  }
  stat = EG_getTessGeom(tess, sizes, &xyzs);
  if (stat != EGADS_SUCCESS) {
    printf(" EG_getTessGeom Face return = %d!\n", stat);
    return 1;
  }
  printf(" Tessellation sizes = %d %d\n", sizes[0], sizes[1]);

plot:
  focus[0] = 0.5*(box[0]+box[3]);
  focus[1] = 0.5*(box[1]+box[4]);
  focus[2] = 0.5*(box[2]+box[5]);
  focus[3] = sqrt( (box[0]-box[3])*(box[0]-box[3]) +
                   (box[1]-box[4])*(box[1]-box[4]) +
                   (box[2]-box[5])*(box[2]-box[5]) );

  gv_init("           View Geometry", mtflag, 0, NULL, NULL, NULL,
          NULL, focus);

  free(xyzi);
  printf(" EG_deleteObject T = %d\n", EG_deleteObject(tess));

  /* make a solid -- assumes wing topology */

  stat = EG_makeFace(geom, SREVERSE, limits, &face);
  printf(" EG_makeFace     F = %d\n", stat);
  /* find closed single edges */
  stat = EG_getTopology(face, &ref, &oclass, &mtype, NULL, &nl,
                        &loops, &ivec);
  printf(" EG_getTopology  F = %d,  nl = %d\n", stat, nl);
  stat = EG_getTopology(loops[0], &ref, &oclass, &mtype, NULL, 
                        &ne, &edges, &senses);
  printf(" EG_getTopology  L = %d,  ne = %d\n", stat, ne);
  for (nc = i = 0; i < ne; i++) {
    stat = EG_getTopology(edges[i], &ref, &oclass, &mtype, limits,
                          &nn, &nodes, &ivec);
    printf(" EG_getTopology  E = %d,   sense = %d  mtype = %d\n", 
           stat, senses[i], mtype);
    if (mtype == DEGENERATE) continue;
    limits[0] = 0.5*(limits[0] + limits[1]);
    EG_evaluate(edges[i], limits, results);
    printf("                     %lf %lf %lf\n",
           results[0], results[1], results[2]);
    if (mtype == ONENODE) nc++;
  }
  printf(" # of caps = %d\n", nc);
  if (nc == 2) {
    for (nc = i = 0; i < ne; i++) {
      stat = EG_getTopology(edges[i], &ref, &oclass, &mtype, NULL,
                            &nn, &nodes, &ivec);
      if (mtype != ONENODE) continue;
      sense = -senses[i];
      stat  = EG_makeTopology(context, NULL, LOOP, CLOSED, NULL, 1, 
                              &edges[i], &sense, &cloop[nc]);
      printf(" EG_makeTopology L = %d\n", stat);
      mtype = SFORWARD;
      if (nc == 1) mtype = SREVERSE;
      stat = EG_makeFace(cloop[nc], mtype, NULL, &cap[nc]);
      printf(" EG_makeFace     L = %d\n", stat);
      nc++;
    }
    /* make shell and solid */
    cap[2] = face;
    stat   = EG_makeTopology(context, NULL, SHELL, CLOSED, NULL, 3, 
                             cap, NULL, &shell);
    printf(" EG_makeTopology S = %d\n", stat);
    sense = 1;    
    stat  = EG_makeTopology(context, NULL, BODY, SOLIDBODY, NULL, 
                            1, &shell, &sense, &solid);
    printf(" EG_makeTopology s = %d\n", stat);
    if (stat == 0) {
      stat = EG_makeTopology(context, NULL, MODEL, 0, NULL, 1, &solid,
                             NULL, &model);
      printf(" EG_makeTopology M = %d\n", stat);
      printf(" EG_saveModel      = %d\n", EG_saveModel(model, "solid.BRep"));
      printf(" \n");
      printf(" EG_deleteObject M = %d\n", EG_deleteObject(model));
    } else {
      printf(" \n");
    }

    printf(" EG_deleteObject S = %d\n", EG_deleteObject(shell));
    printf(" EG_deleteObject F = %d\n", EG_deleteObject(cap[1]));
    printf(" EG_deleteObject L = %d\n", EG_deleteObject(cloop[1]));
    printf(" EG_deleteObject F = %d\n", EG_deleteObject(cap[0]));
    printf(" EG_deleteObject L = %d\n", EG_deleteObject(cloop[0]));
    
  } 

  /* cleanup */
  
  printf(" EG_deleteObject F = %d\n", EG_deleteObject(face));
  printf(" EG_deleteObject G = %d\n", EG_deleteObject(geom));
  printf(" EG_close          = %d\n", EG_close(context));
  return 0;
}


int gvupdate( )
/*
 *  used for single process operation to allow the changing of data
 *
 *  returns: 0 no data has changed
 *           non-zero (positive) data has changed
 *           new number of graphics objects - forces a call to gvdata
 *
 */
{
  return 2;
}


void gvdata( /*@unused@*/ int ngraphics, GvGraphic* graphic[] )
/*
 *  used to (re)set the graphics objects to be used in plotting
 *
 *  ngraphics is the number of objects to be filled
 *                           (from gvupdate or gv_handshake)
 *
 */
{
  int      i;
  GvColor  color;
  GvObject *object;
  
  i           = 0;
  color.red   = 1.0;
  color.green = 0.5;
  color.blue  = 0.0;
  graphic[i]  = gv_alloc(GV_NONINDEXED, GV_QUADMESHS,
                         GV_FOREGROUND|GV_ORIENTATION,
                         color, "Approx Surface  ", 0, 2);
  if (graphic[i] == NULL) {
    printf("gv_alloc ERROR on graphic[%d]\n", i+1);
    return;
  }
  graphic[i]->back.red   = 0.5;
  graphic[i]->back.green = 0.5;
  graphic[i]->back.blue  = 0.5;
  graphic[i]->number     = 1;
  graphic[i]->ddata      = xyzs;
  object = graphic[i]->object;
  object->length = 1;
  object->type.qmeshes.size = (int *) malloc(2*sizeof(int));
  if (object->type.qmeshes.size == NULL) {
    printf("malloc ERROR on graphic[%d]\n", i+1);
    return;      
  }
  object->type.qmeshes.size[0] = sizes[0];
  object->type.qmeshes.size[1] = sizes[1];

  i           = 1;
  color.red   = 1.0;
  color.green = 0.0;
  color.blue  = 0.0;
  graphic[i]  = gv_alloc(GV_NONINDEXED, GV_QUADMESHS,
                         GV_FOREGROUND|GV_ORIENTATION,
                         color, "Input           ", 0, 1);
  if (graphic[i] == NULL) {
    printf("gv_alloc ERROR on graphic[%d]\n", i+1);
    return;
  }
  graphic[i]->back.red   = 0.5;
  graphic[i]->back.green = 0.5;
  graphic[i]->back.blue  = 0.5;
  graphic[i]->number     = 1;
  graphic[i]->ddata      = xyzi;
  object = graphic[i]->object;
  object->length = 1;
  object->type.qmeshes.size = (int *) malloc(2*sizeof(int));
  if (object->type.qmeshes.size == NULL) {
    printf("malloc ERROR on graphic[%d]\n", i+1);
    return;      
  }
  object->type.qmeshes.size[0] = sizei[0];
  object->type.qmeshes.size[1] = sizei[1];
}

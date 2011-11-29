/*
 *      EGADS: Electronic Geometry Aircraft Design System
 *
 *             Make a Solid Wing from a single surface WireFrame
 *
 *      Copyright 2011, Massachusetts Institute of Technology
 *      Licensed under The GNU Lesser General Public License, version 2.1
 *      See http://www.opensource.org/licenses/lgpl-2.1.php
 *
 */

#include "egads.h"


int main(int argc, char *argv[])
{
  int    i, lx, nx, ny, oclass, mtype, per, stat, *ivec;
  int    nl, ne, nc, nn, *senses, sense, sizei[2];
  float  arg;
  char   *str, line[81];
  double tol, limits[4], area, results[18], *rvec, *xyzi;
  ego    context, geom, ref, face, *loops, *edges, *nodes;
  ego    cloop[2], cap[3], shell, solid, model;
  FILE   *fp;

  if (argc != 3) {
    printf("\n Usage: wireWing filename tol\n\n");
    return 1;
  }

  fp = fopen(argv[1], "r");
  if (fp == NULL) {
    printf("\n filename %s Not Found!\n\n", argv[1]);
    return 1;
  }
  
  /* get the wireframe for the wing-like surface */
  
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
  for (i = 0; i < nx*ny; i++) 
    fscanf(fp, "%lf %lf %lf", &xyzi[3*i  ], &xyzi[3*i+1], &xyzi[3*i+2]);
  fclose(fp);
  i--;
  printf(" Last data = %lf %lf %lf\n", xyzi[3*i  ], xyzi[3*i+1], xyzi[3*i+2]);

  sscanf(argv[2], "%f", &arg);
  tol = arg;
  printf(" Tolerance = %lf\n", tol);

  /* initialize */
  printf(" EG_open           = %d\n", EG_open(&context));

  /* approximate the wireframe with a BSpline surface */

  stat = EG_approximate(context, 3, tol, sizei, xyzi, &geom);
  free(xyzi);
  if (stat != EGADS_SUCCESS) {
    printf(" EG_approximate return = %d!\n", stat);
    return 1;
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

  /* make a solid -- assumes wing topology */

  stat = EG_makeFace(geom, SREVERSE, limits, &face);
  printf(" EG_makeFace     F = %d\n", stat);
  /* find closed single edges */
  stat = EG_getTopology(face, &ref, &oclass, &mtype, NULL, &nl,
                        &loops, &ivec);
  printf(" EG_getTopology  F = %d,  nl = %d\n", stat, nl);
  stat = EG_attributeAdd(face, "Source", ATTRSTRING, 0, NULL, NULL, argv[1]);
  printf(" EG_attributeAdd F = %d\n", stat);
  stat = EG_attributeAdd(face, "Name",   ATTRSTRING, 0, NULL, NULL, "Wing");
  printf(" EG_attributeAdd F = %d\n", stat);

  stat = EG_getTopology(loops[0], &ref, &oclass, &mtype, NULL, 
                        &ne, &edges, &senses);
  printf(" EG_getTopology  L = %d,  ne = %d\n", stat, ne);
  for (nc = i = 0; i < ne; i++) {
    stat = EG_getTopology(edges[i], &ref, &oclass, &mtype, limits,
                          &nn, &nodes, &ivec);
    printf(" EG_getTopology  E = %d,   sense = %d  mtype = %d\n", 
           stat, senses[i], mtype);
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
      stat = EG_getArea(cloop[nc], NULL, &area);
      printf(" EG_getArea      L = %d,  %le\n", stat, area);
      mtype = SFORWARD;
      if (area < 0.0) mtype = SREVERSE;
      stat = EG_makeFace(cloop[nc], mtype, NULL, &cap[nc]);
      printf(" EG_makeFace     L = %d\n", stat);
      stat = EG_attributeAdd(cap[nc], "Source",ATTRSTRING,0,NULL,NULL,argv[1]);
      printf(" EG_attributeAdd L = %d\n", stat);
      if (nc == 0) {
        stat = EG_attributeAdd(cap[nc], "Name",ATTRSTRING,0,NULL,NULL,"Tip");
      } else {
        stat = EG_attributeAdd(cap[nc], "Name",ATTRSTRING,0,NULL,NULL,"Root");
      }
      printf(" EG_attributeAdd L = %d\n", stat);
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
      printf(" EG_saveModel      = %d\n", EG_saveModel(model, "wing.egads"));
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

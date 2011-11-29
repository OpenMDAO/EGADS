/*
 *      EGADS: Electronic Geometry Aircraft Design System
 *
 *             Loft a Solid Wing from a single surface WireFrame
 *
 *      Copyright 2011, Massachusetts Institute of Technology
 *      Licensed under The GNU Lesser General Public License, version 2.1
 *      See http://www.opensource.org/licenses/lgpl-2.1.php
 *
 */

#include "egads.h"


int main(int argc, char *argv[])
{
  int    i, j, lx, nx, ny, stat, per, sense, sizei[2] = {0, 0};
  float  arg;
  char   *str, line[81];
  double tol, range[2], save, *xyzi;
  ego    context, *loops, edge, curve, body, model, node;
  FILE   *fp;

  if (argc != 3) {
    printf("\n Usage: wireLoft filename tol\n\n");
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
  nx = lx;
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
  
  loops = (ego *) malloc(ny*sizeof(ego));
  if (loops == NULL) {
    printf(" MALLOC Error!\n\n");
    free(xyzi);
    return 1;
  }
  sizei[0] = nx;

  /* initialize */
  printf(" EG_open           = %d\n", EG_open(&context));

  /* make the appropriate number of wires */
  
  for (i = 0; i < ny; i++) {
    if ((xyzi[3*i*nx  ] != xyzi[3*(i+1)*nx-3]) ||
        (xyzi[3*i*nx+1] != xyzi[3*(i+1)*nx-2]) ||
        (xyzi[3*i*nx+2] != xyzi[3*(i+1)*nx-1])) {
      printf(" Section %d: Not closed!\n", i+1);
      free(loops);
      free(xyzi);
      return 1;
    }
    /* reorder the section */
    for (j = 1; j < nx/2; j++) {
      save = xyzi[3*(i*nx+j)  ];
      xyzi[3*(i*nx+j)  ] = xyzi[3*((i+1)*nx-j)-3];
      xyzi[3*((i+1)*nx-j)-3] = save;
      save = xyzi[3*(i*nx+j)+1];
      xyzi[3*(i*nx+j)+1] = xyzi[3*((i+1)*nx-j)-2];
      xyzi[3*((i+1)*nx-j)-2] = save;
      save = xyzi[3*(i*nx+j)+2];
      xyzi[3*(i*nx+j)+2] = xyzi[3*((i+1)*nx-j)-1];
      xyzi[3*((i+1)*nx-j)-1] = save;
    }
    /* get the curve */
    stat = EG_approximate(context, 3, tol, sizei, &xyzi[3*i*nx], &curve);
    printf(" EG_approximate    = %d\n", stat);
    if (stat != EGADS_SUCCESS) {
      free(loops);
      free(xyzi);
      return 1;
    }
    stat = EG_getRange(curve, range, &per);
    if (stat != EGADS_SUCCESS) {
      printf(" EG_getRange       = %d\n", stat);
      free(loops);
      free(xyzi);
      return 1;
    }
    /* make the loops */
    sense = 1;
    stat  = EG_makeTopology(context, NULL, NODE, 0, &xyzi[3*i*nx],
                            0, NULL, &sense, &node);
    if (stat != EGADS_SUCCESS) {
      printf(" EG_makeTopology N = %d\n", stat);
      free(loops);
      free(xyzi);
      return 1;
    }
    stat = EG_makeTopology(context, curve, EDGE, ONENODE, range,
                           1, &node, &sense, &edge);
    if (stat != EGADS_SUCCESS) {
      printf(" EG_makeTopology E = %d\n", stat);
      free(loops);
      free(xyzi);
      return 1;
    }
    stat = EG_makeTopology(context, NULL, LOOP, CLOSED, NULL,
                           1, &edge, &sense, &loops[i]);
    if (stat != EGADS_SUCCESS) {
      printf(" EG_makeTopology L = %d\n", stat);
      free(loops);
      free(xyzi);
      return 1;
    }
  }
  free(xyzi);

  /* do the loft */
  stat = EG_loft(ny, loops, 0, &body);
  if (stat == 0) {
    stat = EG_makeTopology(context, NULL, MODEL, 0, NULL, 1, &body,
                           NULL, &model);
    printf(" EG_makeTopology M = %d\n", stat);
    printf(" EG_saveModel      = %d\n", EG_saveModel(model, "loft.egads"));
    printf(" \n");
    printf(" EG_deleteObject M = %d\n", EG_deleteObject(model));
  } else {
    printf(" EG_loft           = %d\n", stat);
    printf(" \n");
  }

  /* cleanup */
  free(loops);
  printf(" EG_deleteObject   = %d\n", EG_deleteObject(context));
  printf(" EG_close          = %d\n", EG_close(context));
  return 0;
}

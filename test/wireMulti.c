/*
 *      EGADS: Electronic Geometry Aircraft Design System
 *
 *             Make a Multi-Panel Solid Wing from a set of
 *                      surface WireFrames
 *
 *      Copyright 2011-2012, Massachusetts Institute of Technology
 *      Licensed under The GNU Lesser General Public License, version 2.1
 *      See http://www.opensource.org/licenses/lgpl-2.1.php
 *
 */

#include <math.h>
#include "egads.h"


typedef struct {
  int    sizei[2];              /* size of input wireframe */
  double *xyzi;                 /* the wireframe data */
  ego    geom;                  /* constructed geometry */
  ego    face;                  /* the constructed face */
  int    left;                  /* edge position for left (prev) */
  int    rite;                  /* edge position for right (next) */
  int    prev;                  /* previous panel index */
  int    next;                  /* next panel index */
} Panel;



int main(int argc, char *argv[])
{
  int    i, j, k, lx, nx, ny, oclass, mtype, per, stat, n, nn;
  int    nface, ipanel, ne, sense, esen, snew[4], *senses, *ivec;
  char   *str, line[81];
  double limits[4], params[2], xyz[3], *rvec, sum, area, tol = 1.e-5;
  Panel  *panels;
  ego    context, ref, prev, next, *faces, *loops, *edges, *nodes;
  ego    onode, node, edge, shell, solid, model, enew[8], cloop;
  ego    loop, geom, nnew[2];
  FILE   *fp;

  if (argc < 2) {
    printf("\n Usage: wireMulti filename1 filename2 ... \n\n");
    return 1;
  }
  
  panels = (Panel *) malloc((argc-1)*sizeof(Panel));
  if (panels == NULL) {
    printf(" Cannot Allocate %d Panels!\n\n", argc-1);
    return 1;
  }

  for (j = 0; j < argc-1; j++) {
    fp = fopen(argv[j+1], "r");
    if (fp == NULL) {
      printf("\n filename %s Not Found!\n\n", argv[j+1]);
      for (i = 0; i < j; i++) free(panels[i].xyzi);
      free(panels);
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
            for (i = 0; i < j; i++) free(panels[i].xyzi);
            free(panels);
            return 1;
          }
        lx = nx;
        nx = 0;
        ny++;
      } else {
        nx++;
      }
    } while (str != NULL);
    panels[j].sizei[0] = nx = lx;
    panels[j].sizei[1] = ny;
    printf(" Panel #%d: nx, ny = %d %d\n", j+1, nx, ny);
    if (j != 0)
      if (panels[j].sizei[0] != panels[j-1].sizei[0]) {
        printf(" Panel #%d nx = %d,  Panel #%d nx = %d\n\n",
               j, panels[j-1].sizei[0], j+1, panels[j].sizei[0]);
        for (i = 0; i < j; i++) free(panels[i].xyzi);
        free(panels);
        return 1;
      }
    panels[j].xyzi = (double *) malloc(nx*ny*3*sizeof(double));
    if (panels[j].xyzi == NULL) {
      printf(" MALLOC Error!\n\n");
      for (i = 0; i < j; i++) free(panels[i].xyzi);
      free(panels);
      return 1;
    }
    rewind(fp);
    for (i = 0; i < nx*ny; i++) 
      fscanf(fp, "%lf %lf %lf",      &panels[j].xyzi[3*i  ], 
             &panels[j].xyzi[3*i+1], &panels[j].xyzi[3*i+2]);
    fclose(fp);
    i--;
    printf(" Panel #%d: Last data = %lf %lf %lf\n", j+1,
           panels[j].xyzi[3*i  ], panels[j].xyzi[3*i+1], 
           panels[j].xyzi[3*i+2]);
    panels[j].prev = panels[j].next = -1;
    panels[j].left = panels[j].rite = -1;
  }

  /* order the panels by matching first and last curves */
  nx = panels[0].sizei[0];
  for (j = 0; j < argc-2; j++) 
    for (i = j+1; i < argc-1; i++) {
      lx  = 3*nx*(panels[j].sizei[1]-1);
      ny  = 3*nx*(panels[i].sizei[1]-1);
      sum = 0.0;
      for (k = 0; k < nx; k++) 
        sum += sqrt((panels[j].xyzi[3*k  ]-panels[i].xyzi[3*k  ])*
                    (panels[j].xyzi[3*k  ]-panels[i].xyzi[3*k  ])  + 
                    (panels[j].xyzi[3*k+1]-panels[i].xyzi[3*k+1])*
                    (panels[j].xyzi[3*k+1]-panels[i].xyzi[3*k+1])  +
                    (panels[j].xyzi[3*k+2]-panels[i].xyzi[3*k+2])*
                    (panels[j].xyzi[3*k+2]-panels[i].xyzi[3*k+2]));
      if (sum < tol) {
        panels[j].prev = i;
        panels[i].prev = j;
      }
/*    printf(" %d/%d left/left sum = %le\n", j, i, sum);  */
      sum = 0.0;
      for (k = 0; k < nx; k++) 
        sum += sqrt((panels[j].xyzi[lx+3*k  ]-panels[i].xyzi[3*k  ])*
                    (panels[j].xyzi[lx+3*k  ]-panels[i].xyzi[3*k  ])  + 
                    (panels[j].xyzi[lx+3*k+1]-panels[i].xyzi[3*k+1])*
                    (panels[j].xyzi[lx+3*k+1]-panels[i].xyzi[3*k+1])  +
                    (panels[j].xyzi[lx+3*k+2]-panels[i].xyzi[3*k+2])*
                    (panels[j].xyzi[lx+3*k+2]-panels[i].xyzi[3*k+2]));
      if (sum < tol) {
        panels[j].next = i;
        panels[i].prev = j;
      }
/*    printf(" %d/%d rite/left sum = %le\n", j, i, sum);  */
      sum = 0.0;
      for (k = 0; k < nx; k++) 
        sum += sqrt((panels[j].xyzi[3*k  ]-panels[i].xyzi[ny+3*k  ])*
                    (panels[j].xyzi[3*k  ]-panels[i].xyzi[ny+3*k  ])  + 
                    (panels[j].xyzi[3*k+1]-panels[i].xyzi[ny+3*k+1])*
                    (panels[j].xyzi[3*k+1]-panels[i].xyzi[ny+3*k+1])  +
                    (panels[j].xyzi[3*k+2]-panels[i].xyzi[ny+3*k+2])*
                    (panels[j].xyzi[3*k+2]-panels[i].xyzi[ny+3*k+2]));
      if (sum < tol) {
        panels[j].prev = i;
        panels[i].next = j;
      }
/*    printf(" %d/%d left/rite sum = %le\n", j, i, sum);  */
      sum = 0.0;
      for (k = 0; k < nx; k++) 
        sum += sqrt((panels[j].xyzi[lx+3*k  ]-panels[i].xyzi[ny+3*k  ])*
                    (panels[j].xyzi[lx+3*k  ]-panels[i].xyzi[ny+3*k  ])  + 
                    (panels[j].xyzi[lx+3*k+1]-panels[i].xyzi[ny+3*k+1])*
                    (panels[j].xyzi[lx+3*k+1]-panels[i].xyzi[ny+3*k+1])  +
                    (panels[j].xyzi[lx+3*k+2]-panels[i].xyzi[ny+3*k+2])*
                    (panels[j].xyzi[lx+3*k+2]-panels[i].xyzi[ny+3*k+2]));
      if (sum < tol) {
        panels[j].next = i;
        panels[i].next = j;
      }
/*    printf(" %d/%d rite/rite sum = %le\n", j, i, sum);  */
    }
  for (lx = ny = j = 0; j < argc-1; j++) {
    if ((panels[j].prev == -1) && (panels[j].next == -1)) {
      printf(" Panel %d: Not connected!\n\n", j+1);
      for (i = 0; i < argc-1; i++) free(panels[i].xyzi);
      free(panels);
      return -1;
    }
    if (panels[j].prev == -1) lx++;
    if (panels[j].next == -1) ny++;
  }
  if ((ny != 1) || (lx != 1)) {
    printf(" Open Panels = %d %d\n\n", lx, ny);
    for (i = 0; i < argc-1; i++) free(panels[i].xyzi);
    free(panels);
    return -1;
  }
  printf(" Tolerance = %lf\n", tol);

  /* initialize EGADS */
  
  printf(" EG_open           = %d\n", EG_open(&context));

  /* approximate the wireframes with a BSpline surface */

  for (j = 0; j < argc-1; j++) {
    lx   = 3*nx*(panels[j].sizei[1]-1);
    stat = EG_approximate(context, 3, tol, panels[j].sizei, 
                          panels[j].xyzi, &panels[j].geom);
    if (stat != EGADS_SUCCESS) {
      printf(" Panel #%d: EG_approximate return = %d!\n",
             j+1, stat);
      for (i = j; i < argc-1; i++) free(panels[i].xyzi);
      free(panels);
      return 1;
    }
    stat = EG_getGeometry(panels[j].geom, &oclass, &mtype, 
                          &ref, &ivec, &rvec);
    if (stat != EGADS_SUCCESS) {
      printf(" Panel #%d: EG_getGeometry return = %d\n", 
             j+1, stat);
      for (i = j; i < argc-1; i++) free(panels[i].xyzi);
      free(panels);
      return 1;
    }
    printf(" Panel #%d: %x - U deg = %d #CPs = %d #knots = %d ",
           j+1, ivec[0], ivec[1], ivec[2], ivec[3]);
    printf("- V deg = %d #CPs = %d #knots = %d\n",
           ivec[4], ivec[5], ivec[6]);
    EG_free(ivec);
    EG_free(rvec);
    stat = EG_getRange(panels[j].geom, limits, &per);
    if (stat != EGADS_SUCCESS) {
      printf(" Panel #%d: EG_getRange return = %d\n", j+1, stat);
      for (i = j; i < argc-1; i++) free(panels[i].xyzi);
      free(panels);
      return 1;
    }
    stat = EG_makeFace( panels[j].geom, SREVERSE, limits, 
                       &panels[j].face);
    printf(" Panel %d: EG_makeFace     F = %d\n", j+1, stat);
    
    /* find the edges assoctated with the bounds of the patch */
    stat = EG_getTopology(panels[j].face, &ref, &oclass, &mtype, limits,
                          &n, &loops, &senses);
    stat = EG_getTopology(loops[0], &ref, &oclass, &mtype, limits,
                          &n, &edges, &senses);
    for (i = 0; i < n; i++) {
      EG_getInfo(edges[i], &oclass, &mtype, &ref, &prev, &next);
      if (mtype != ONENODE) continue;
      sum = 0.0;
      for (k = 0; k < nx; k++) {
        EG_invEvaluate(edges[i], &panels[j].xyzi[3*k], params, xyz); 
        sum += sqrt((panels[j].xyzi[3*k  ]-xyz[0])*
                    (panels[j].xyzi[3*k  ]-xyz[0])  + 
                    (panels[j].xyzi[3*k+1]-xyz[1])*
                    (panels[j].xyzi[3*k+1]-xyz[1])  +
                    (panels[j].xyzi[3*k+2]-xyz[2])*
                    (panels[j].xyzi[3*k+2]-xyz[2]));
      }
      if (sum < tol) panels[j].left = i;
/*    printf(" Panel #%d:  Edge %d  left  sum = %le\n", j+1, i, sum);  */
      sum = 0.0;
      for (k = 0; k < nx; k++) {
        EG_invEvaluate(edges[i], &panels[j].xyzi[lx+3*k], params, xyz); 
        sum += sqrt((panels[j].xyzi[lx+3*k  ]-xyz[0])*
                    (panels[j].xyzi[lx+3*k  ]-xyz[0])  + 
                    (panels[j].xyzi[lx+3*k+1]-xyz[1])*
                    (panels[j].xyzi[lx+3*k+1]-xyz[1])  +
                    (panels[j].xyzi[lx+3*k+2]-xyz[2])*
                    (panels[j].xyzi[lx+3*k+2]-xyz[2]));
      }
      if (sum < tol) panels[j].rite = i;
/*    printf(" Panel #%d:  Edge %d  right sum = %le\n", j+1, i, sum);  */
    }
    free(panels[j].xyzi);
    panels[j].xyzi = NULL;
  }
  for (j = 0; j < argc-1; j++) {
    if ((panels[j].left == -1) || (panels[j].rite == -1)) {
      printf(" Panel #%d: Edge Markers = %d  %d\n\n", j+1,
             panels[j].left, panels[j].rite);
      free(panels);
      return 1;
    }
  }
  printf("\n");
  
  /* make the solid - start with the first face */
  
  nface = argc+1;
  faces = (ego *) malloc(nface*sizeof(ego));
  if (faces == NULL) {
    printf(" Cant Allocate %d egos!\n\n", nface);
    free(panels);
    return 1;
  }
  ipanel = -1;
  for (j = 0; j < argc-1; j++)
    if (panels[j].prev == -1) {
      ipanel = j;
      break;
    }
  if (ipanel == -1) {
    printf(" No Start!\n\n");
    free(faces);
    free(panels);
    return 1;
  }
  faces[1] = panels[ipanel].face;
  stat = EG_getTopology(faces[1], &ref, &oclass, &mtype, NULL, &n,
                        &loops, &senses);
  printf(" EG_getTopology  F = %d,  nLoop = %d\n", stat, n);
  stat = EG_attributeAdd(faces[1], "Source", ATTRSTRING, 0, NULL, NULL, 
                         "wireMulti");
  printf(" EG_attributeAdd F = %d\n", stat);
  stat = EG_attributeAdd(faces[1], "Name",   ATTRSTRING, 0, NULL, NULL, 
                         argv[ipanel+1]);
  printf(" EG_attributeAdd F = %d\n", stat);
  stat = EG_getTopology(loops[0], &ref, &oclass, &mtype, NULL, 
                        &ne, &edges, &senses);
  printf(" EG_getTopology  L = %d,  ne = %d\n", stat, ne);
  i     = panels[ipanel].rite;
  stat  = EG_getTopology(edges[i], &ref, &oclass, &mtype, NULL,
                         &nn, &nodes, &ivec);
  edge  =  edges[i];
  esen  = -senses[i];
  node  =  nodes[0];
  printf(" Panel #%d: Complete (open to %d)!\n\n", ipanel+1, i);

  i     = panels[ipanel].left;
  stat  = EG_getTopology(edges[i], &ref, &oclass, &mtype, NULL,
                         &nn, &nodes, &ivec);
  sense = -senses[i];
  stat  = EG_makeTopology(context, NULL, LOOP, CLOSED, NULL, 1, 
                          &edges[i], &sense, &cloop);
  printf(" EG_makeTopology L = %d\n", stat);
  stat = EG_getArea(cloop, NULL, &area);
  printf(" EG_getArea      L = %d,  %le\n", stat, area);
  mtype = SFORWARD;
  if (area < 0.0) mtype = SREVERSE;
  stat = EG_makeFace(cloop, mtype, NULL, &faces[0]);
  printf(" EG_makeFace Cap F = %d\n", stat);
  stat = EG_attributeAdd(faces[0], "Source", ATTRSTRING, 0, NULL, NULL,
                         "wireMulti");
  printf(" EG_attributeAdd F = %d\n", stat);
  stat = EG_attributeAdd(faces[0], "Name", ATTRSTRING, 0, NULL, NULL,
                         "FirstCap");
  printf(" EG_attributeAdd F = %d\n", stat);
  printf(" Panel #%d: %d First Capped!\n\n", ipanel+1, i);
  nface = 2;
 
  /* loop over and recreate all but the first face */
  
  while (panels[ipanel].next != -1) {
    ipanel = panels[ipanel].next;
    stat   = EG_getTopology(panels[ipanel].face, &geom, &oclass, 
                            &mtype, NULL, &n, &loops, &senses);
    printf(" EG_getTopology  F = %d,  nLoop = %d\n", stat, n);
    stat   = EG_getTopology(loops[0], &ref, &oclass, &mtype, NULL, 
                            &ne, &edges, &senses);
    printf(" EG_getTopology  L = %d,  nEdge = %d\n", stat, ne);
    for (j = 0; j < 4; j++) {
      snew[j]   = senses[j];
      enew[j]   = edges[j];
      enew[j+4] = edges[j+4];
    }
    i       = panels[ipanel].left;
    enew[i] = edge;                     /* last panel edge */
    snew[i] = esen;
    stat = EG_otherCurve(geom, edge, tol, &enew[i+4]);
    printf(" EG_otherCurve     = %d\n", stat);
    stat  = EG_getTopology(edges[i], &ref, &oclass, &mtype, NULL,
                           &nn, &nodes, &ivec);
    onode = nodes[0];

    for (j = 0; j < 4; j++) {
      if (j == i) continue;
      stat = EG_getTopology(enew[j], &ref, &oclass, &mtype, limits,
                            &nn, &nodes, &ivec);
      k = 0;
      if (EG_isEquivalent(nodes[0], onode) == EGADS_SUCCESS) k++;
      if (mtype == TWONODE)
        if (EG_isEquivalent(nodes[1], onode) == EGADS_SUCCESS) k++;
      if (k == 0) continue;
      /* hit a duplicate of our node -- remake the edge */
      nnew[0] = nodes[0];
      if (EG_isEquivalent(nodes[0], onode) == EGADS_SUCCESS) 
        nnew[0] = node;
      if (mtype == TWONODE) {
        nnew[1] = nodes[1];
        if (EG_isEquivalent(nodes[1], onode) == EGADS_SUCCESS) 
          nnew[1] = node;
      }
      stat = EG_makeTopology(context, ref, oclass, mtype, limits,
                             nn, nnew, NULL, &enew[j]);
      printf(" EG_makeTopology E = %d   %d/%d\n", stat, ipanel+1, j);

      for (k = j+1; k < 4; k++)
        if (EG_isEquivalent(edges[j], edges[k]) == EGADS_SUCCESS) {
          enew[k] = enew[j];
          printf("      Found Match %d %d  senses = %d %d\n", 
                 j, k, snew[j], snew[k]);
          break;
        }

    }
    stat = EG_getTopology(loops[0], &ref, &oclass, &mtype, NULL, 
                          &ne, &edges, &senses);
    printf(" EG_getTopology  L = %d,  nEdge = %d\n", stat, ne);
    stat = EG_makeTopology(context, ref, oclass, mtype, NULL,
                           ne, enew, snew, &loop);
    printf(" EG_makeTopology L = %d\n", stat);
    stat = EG_getTopology(panels[ipanel].face, &ref, &oclass, &mtype, 
                          limits, &n, &loops, &senses);
    printf(" EG_getTopology  F = %d\n", stat);
    sense = 1;
    stat  = EG_makeTopology(context, ref, oclass, mtype, limits,
                            1, &loop, &sense, &faces[nface]);
    printf(" EG_makeTopology F = %d   %d\n", stat, mtype);
    stat = EG_attributeAdd(faces[nface], "Source", ATTRSTRING, 0, NULL,
                           NULL, "wireMulti");
    printf(" EG_attributeAdd F = %d\n", stat);
    stat = EG_attributeAdd(faces[nface], "Name",   ATTRSTRING, 0, NULL, 
                           NULL, argv[ipanel+1]);
    printf(" EG_attributeAdd F = %d\n", stat);
    printf(" Panel #%d: %d Complete!\n\n", ipanel+1, i);
    i     = panels[ipanel].rite;
    stat  = EG_getTopology(enew[i], &ref, &oclass, &mtype, NULL,
                           &nn, &nodes, &ivec);
    edge  =  enew[i];
    esen  = -snew[i];
    node  =  nodes[0];
    nface++;
  }

  /* cap off the end */
  
  stat = EG_getTopology(faces[nface-1], &ref, &oclass, &mtype, NULL, 
                        &n, &loops, &senses);
  printf(" EG_getTopology  F = %d,  nLoop = %d\n", stat, n);
  stat = EG_getTopology(loops[0], &ref, &oclass, &mtype, NULL, 
                        &ne, &edges, &senses);
  i     = panels[ipanel].rite;
  stat  = EG_getTopology(edges[i], &ref, &oclass, &mtype, NULL,
                         &nn, &nodes, &ivec);
  sense = -senses[i];
  stat  = EG_makeTopology(context, NULL, LOOP, CLOSED, NULL, 1, 
                          &edges[i], &sense, &cloop);
  printf(" EG_makeTopology L = %d\n", stat);
  stat = EG_getArea(cloop, NULL, &area);
  printf(" EG_getArea      L = %d,  %le\n", stat, area);
  mtype = SFORWARD;
  if (area < 0.0) mtype = SREVERSE;
  stat = EG_makeFace(cloop, mtype, NULL, &faces[nface]);
  printf(" EG_makeFace Cap F = %d\n", stat);
  stat = EG_attributeAdd(faces[nface], "Source", ATTRSTRING, 0, NULL, NULL,
                         "wireMulti");
  printf(" EG_attributeAdd F = %d\n", stat);
  stat = EG_attributeAdd(faces[nface], "Name", ATTRSTRING, 0, NULL, NULL,
                         "EndCap");
  printf(" EG_attributeAdd F = %d\n", stat);
  printf(" Panel #%d: %d End Capped!\n\n", ipanel+1, i);
  nface++;
  
  /* make shell and solid */

  stat = EG_makeTopology(context, NULL, SHELL, CLOSED, NULL, nface, 
                         faces, NULL, &shell);
  printf(" EG_makeTopology S = %d\n", stat);
  sense = 1;    
  stat  = EG_makeTopology(context, NULL, BODY, SOLIDBODY, NULL, 
                          1, &shell, &sense, &solid);
  printf(" EG_makeTopology s = %d\n", stat);
  if (stat == 0) {
    stat = EG_makeTopology(context, NULL, MODEL, 0, NULL, 1, &solid,
                           NULL, &model);
    printf(" EG_makeTopology M = %d\n", stat);
    printf(" EG_saveModel      = %d\n", EG_saveModel(model, "multi.egads"));
    printf(" EG_deleteObject M = %d\n", EG_deleteObject(model));
  }
  printf(" \n");
  
  /* cleanup */
  
  printf(" EG_deleteObject C = %d\n", EG_deleteObject(context));
  free(faces);
  free(panels);
  printf(" EG_close          = %d\n", EG_close(context));
  return 0;
}

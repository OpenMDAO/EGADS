/*
 *      EGADS: Electronic Geometry Aircraft Design System
 *
 *             WireBody Tessellation Test
 *
 *      Copyright 2011-2012, Massachusetts Institute of Technology
 *      Licensed under The GNU Lesser General Public License, version 2.1
 *      See http://www.opensource.org/licenses/lgpl-2.1.php
 *
 */

#include <math.h>
#include "egads.h"


int main(/*@unused@*/int argc, /*@unused@*/char *argv[])
{
  int    i, j, n, stat, mtype, oclass, nbody, nface, nedge, npts, *senses;
  double box[6], params[3], size;
  ego    context, model, tess, geom, body, obj, *bodies, *faces, *loops, *edges;
  const double *xyzs, *ts;
  
  /* initialize */
  printf(" EG_open         = %d\n", EG_open(&context));
  printf(" EG_loadModel    = %d\n", EG_loadModel(context, 0, "wing.egads", 
                                                 &model));
  /* get all bodies */
  printf(" EG_getTopology  = %d\n", EG_getTopology(model, &geom, &oclass, 
                                                   &mtype, NULL, &nbody,
                                                   &bodies, &senses));
  printf(" EG_getBBox      = %d\n", EG_getBoundingBox(bodies[0], box));
  size = sqrt((box[0]-box[3])*(box[0]-box[3]) +
              (box[1]-box[4])*(box[1]-box[4]) +
              (box[2]-box[5])*(box[2]-box[5]));

  /* get all faces in body */
  printf(" EG_getBodyTopos = %d\n", EG_getBodyTopos(bodies[0], NULL, FACE,
                                                    &nface, &faces));
  /* look at third face */
  obj = faces[2];
  printf(" EG_getTopology  = %d\n", EG_getTopology(obj, &geom, &oclass, 
                                                   &mtype, NULL, &n,
                                                   &loops, &senses));
  printf(" EG_makeTopology = %d\n", EG_makeTopology(context, NULL, BODY,
                                                    WIREBODY, NULL, 1,
                                                    loops, NULL, &body));
  printf(" EG_makeTopology = %d\n", EG_makeTopology(context, NULL, MODEL,
                                                    0, NULL, 1, &body, NULL, 
                                                    &obj));
  printf(" \n");

  params[0] =  0.050*size;
  params[1] =  0.001*size;
  params[2] = 15.0;
  printf(" EG_makeTessBody = %d\n", EG_makeTessBody(body, params, &tess));
  printf(" EG_getBodyTopos = %d\n", EG_getBodyTopos(body, NULL, EDGE,
                                                    &nedge, &edges));
  for (i = 0; i < nedge; i++) {
    stat = EG_getTessEdge(tess, i+1, &npts, &xyzs, &ts);
    printf("\n Edge %d   npts = %d,  stat = %d\n", i+1, npts, stat);
    if (stat != EGADS_SUCCESS) continue;
    for (j = 0; j < npts; j++)
      printf(" %lf   %lf %lf %lf\n", 
             ts[j], xyzs[3*j  ], xyzs[3*j+1], xyzs[3*j+2]);
  }
  printf("\n");

  printf(" EG_deleteObject = %d\n", EG_deleteObject(tess));
  printf(" EG_deleteObject = %d\n", EG_deleteObject(obj));
  EG_free(faces);
  printf(" EG_deleteObject = %d\n", EG_deleteObject(model));
  printf(" EG_close        = %d\n", EG_close(context));
  return 0;
}

/*
 *      EGADS: Electronic Geometry Aircraft Design System
 *
 *             reTessellate a list of Faces
 *
 *      Copyright 2011-2013, Massachusetts Institute of Technology
 *      Licensed under The GNU Lesser General Public License, version 2.1
 *      See http://www.opensource.org/licenses/lgpl-2.1.php
 *
 */

#include <stdio.h>
#include <stdlib.h>
#include <math.h>

#include "egads.h"


#define PI               3.14159265358979324


  /*** internal function not in egads.h ***/
  extern int EG_outLevel(const egObject *object);


static int
EG_retessEdge(egObject *tess, int eIndex, egObject *edge, double *params)
{
  int    i, j, k, stat, npts;
  double d, dist2, dotnrm, tang[3], result[9], *newts;
  const double *xyz, *t;
 
  /* insert based on length */
  if (params[0] > 0.0)
    for (;;) {
      stat = EG_getTessEdge(tess, eIndex, &npts, &xyz, &t);
      if (stat != EGADS_SUCCESS) return stat;
      if (npts < 2) return EGADS_EMPTY;
      i     = 0;
      dist2 = (xyz[0]-xyz[3])*(xyz[0]-xyz[3]) +
              (xyz[1]-xyz[4])*(xyz[1]-xyz[4]) +
              (xyz[2]-xyz[5])*(xyz[2]-xyz[5]);
      for (j = 1; j < npts-1; j++) {
        d = (xyz[3*j  ]-xyz[3*j+3])*(xyz[3*j  ]-xyz[3*j+3]) +
            (xyz[3*j+1]-xyz[3*j+4])*(xyz[3*j+1]-xyz[3*j+4]) +
            (xyz[3*j+2]-xyz[3*j+5])*(xyz[3*j+2]-xyz[3*j+5]);
        if (d > dist2) {
          i     = j;
          dist2 = d;
        }
      }
      if (dist2 <= params[0]*params[0]) break;

      /* number of insertion points */
      j     = sqrt(dist2)/params[0];
      newts = (double *) EG_alloc(j*sizeof(double));
      if (newts == NULL) return EGADS_MALLOC;
      for (k = 0; k < j; k++)
        newts[k] = t[i] + (k+1.0)*(t[i+1]-t[i])/(j+1.0);
      stat = EG_insertEdgeVerts(tess, eIndex, i+1, j, newts);
      EG_free(newts);
      if (stat != EGADS_SUCCESS) return stat;
    }
  
  /* sag */
  if (params[1] > 0.0)
    for (;;) {
      stat = EG_getTessEdge(tess, eIndex, &npts, &xyz, &t);
      if (stat != EGADS_SUCCESS) return stat;
      if (npts < 2) return EGADS_EMPTY;
      i = -1;
      for (j = 0; j < npts-1; j++) {
        d    = 0.5*(t[j]+t[j+1]);
        stat = EG_evaluate(edge, &d, result);
        if (stat != EGADS_SUCCESS) return stat;
        result[3] = 0.5*(xyz[3*j  ] + xyz[3*j+3]);
        result[4] = 0.5*(xyz[3*j+1] + xyz[3*j+4]);
        result[5] = 0.5*(xyz[3*j+2] + xyz[3*j+5]);
        d = (result[0]-result[3])*(result[0]-result[3]) +
            (result[1]-result[4])*(result[1]-result[4]) +
            (result[2]-result[5])*(result[2]-result[5]);
        if (i == -1) {
          dist2 = d;
          i     = j;
        } else {
          if (d > dist2) {
            dist2 = d;
            i     = j;
          }
        }
      }
      if (dist2 <= params[1]*params[1]) break;

      /* insert one vertex in the segment with the largest chordal deviation */
      d    = 0.5*(t[i]+t[i+1]);
      stat = EG_insertEdgeVerts(tess, eIndex, i+1, 1, &d);
      if (stat != EGADS_SUCCESS) return stat;
    }
  
  /* angle */
  if (params[2] > 0.0) {
    d = params[2];
    if (d < 0.5) d = 0.5;
    dotnrm = cos(PI*d/180.0);
    for (;;) {
      stat = EG_getTessEdge(tess, eIndex, &npts, &xyz, &t);
      if (stat != EGADS_SUCCESS) return stat;
      if (npts < 2) return EGADS_EMPTY;
      stat = EG_evaluate(edge, &t[0], result);
      if (stat != EGADS_SUCCESS) return stat;
      d = sqrt(result[3]*result[3] + result[4]*result[4] +
               result[5]*result[5]);
      if (d == 0.0) d = 1.0;
      tang[0] = result[3]/d;
      tang[1] = result[4]/d;
      tang[2] = result[5]/d;
      dist2   = 1.0;
      for (j = 0; j < npts-1; j++) {
        stat = EG_evaluate(edge, &t[j+1], result);
        if (stat != EGADS_SUCCESS) return stat;
        d = sqrt(result[3]*result[3] + result[4]*result[4] +
                 result[5]*result[5]);
        if (d == 0.0) d = 1.0;
        result[3] /= d;
        result[4] /= d;
        result[5] /= d;
        d = tang[0]*result[3] + tang[1]*result[4] + tang[2]*result[5];
        if (d < dist2) {
          dist2 = d;
          i     = j;
        }
        tang[0] = result[3];
        tang[1] = result[4];
        tang[2] = result[5];
      }
      if (dist2 >= dotnrm) break;

      /* insert one vertex in the segment with the largest tangent deviation */
      d    = 0.5*(t[i]+t[i+1]);
      stat = EG_insertEdgeVerts(tess, eIndex, i+1, 1, &d);
      if (stat != EGADS_SUCCESS) return stat;      
    }
  }

  return EGADS_SUCCESS;
}


/* 
 * redoes the discretization for the list of face indices and
 *        adjusts ALL touching edge tessellations to be consistent
 *
 * where: tess    the tessellation object to modify
 *        nf      the number of faces
 *        iface   the face indices
 *        param   the new parameters to use
 *
 */
 
int
EG_retessFaces(egObject *tess, int nf, int *iface, double *params)
{
  int      i, j, k, stat, oclass, mtype, outLevel, nface, nedge, *iedge, nef;
  egObject *object, *topRef, *next, *prev, **faces, **edges, **edgf, **facedg;
  egTessel *btess;

  if (tess == NULL)                 return EGADS_NULLOBJ;
  if (tess->magicnumber != MAGIC)   return EGADS_NOTOBJ;
  if (tess->oclass != TESSELLATION) return EGADS_NOTTESS;
  if (tess->blind == NULL)          return EGADS_NODATA;
  btess  = tess->blind;
  object = btess->src;
  if (object == NULL)               return EGADS_NULLOBJ;
  if (object->magicnumber != MAGIC) return EGADS_NOTOBJ;
  if (object->oclass != BODY)       return EGADS_NOTBODY;
  if (nf <= 0)                      return EGADS_NODATA;
  if (iface == NULL)                return EGADS_NODATA;
  outLevel = EG_outLevel(object);
  
  /* get all of the face objects in the body */
  stat = EG_getBodyTopos(object, NULL, FACE, &nface, &faces);
  if (stat != EGADS_SUCCESS) {
    if (outLevel > 0)
      printf(" EGADS Error: EG_getBodyTopos(F) = %d (EG_retessFaces)!\n",
             stat);
    return stat;
  }
  
  /* check our face indices */
  for (i = 0; i < nf; i++) {
    if ((iface[i] < 1) || (iface[i] > nface)) {
      if (outLevel > 0)
        printf(" EGADS Error: iface %d = %d [1-%d] (EG_retessFaces)!\n",
               i+1, iface[i], nface);
      EG_free(faces);
      return EGADS_INDEXERR;
    }
    for (j = 0; j < i; j++)
      if (iface[i] == iface[j]) {
        if (outLevel > 0)
          printf(" EGADS Error: iface %d == iface %d (EG_retessFaces)!\n",
                 i+1, j+1);
        EG_free(faces);
        return EGADS_INDEXERR;
      }
  }

  /* get all of the edge objects in the body */
  stat = EG_getBodyTopos(object, NULL, EDGE, &nedge, &edges);
  if (stat != EGADS_SUCCESS) {
    if (outLevel > 0)
      printf(" EGADS Error: EG_getBodyTopos(E) = %d (EG_retessFaces)!\n",
             stat);
    EG_free(faces);
    return stat;
  }
  
  /* mark the edges as to the number of faces in the list */
  iedge = (int *) EG_alloc(nedge*sizeof(int));
  if (iedge == NULL) {
    if (outLevel > 0)
      printf(" EGADS Error: Allocation on %d indices (EG_retessFaces)!\n",
             nedge);
    EG_free(edges);
    EG_free(faces);
    return EGADS_MALLOC;
  }
  for (i = 0; i < nedge; i++) {
    iedge[i] = 0;
    stat = EG_getInfo(edges[i], &oclass, &mtype, &topRef, &prev, &next);
    if (stat != EGADS_SUCCESS) {
      if (outLevel > 0)
        printf(" EGADS Error: EG_getInfo Edge %d = %d (EG_retessFaces)!\n",
               i+1, stat);
      EG_free(iedge);
      EG_free(edges);
      EG_free(faces);
      return stat;
    }
    if (mtype == DEGENERATE) iedge[i] = -1;
  }
  for (i = 0; i < nf; i++) {
    stat = EG_getBodyTopos(object, faces[iface[i]-1], EDGE, &nef, &edgf);
    if (stat != EGADS_SUCCESS) {
      if (outLevel > 0)
        printf(" EGADS Error: EG_getBodyTopos(EF) %d = %d (EG_retessFaces)!\n",
               iface[i], stat);
      EG_free(iedge);
      EG_free(edges);
      EG_free(faces);
      return stat;
    }
    for (j = 0; j < nef; j++) {
      k = EG_indexBodyTopo(object, edgf[j]);
      if (k <= EGADS_SUCCESS) {
        if (outLevel > 0)
          printf(" EGADS Error: EG_indexBodyTopo %d/%d=%d (EG_retessFaces)!\n",
                 i+1, j+1, stat);
        EG_free(iedge);
        EG_free(edges);
        EG_free(faces);
        if (stat == EGADS_SUCCESS) stat = EGADS_INDEXERR;
        return stat;
      }
      if (iedge[k-1] >= 0) iedge[k-1]++;
    }
    EG_free(edgf);
  }
  
  facedg = (egObject **) EG_alloc((nf+nedge)*sizeof(egObject *));
  if (facedg == NULL) {
    if (outLevel > 0)
      printf(" EGADS Error: Allocation on %d objects (EG_retessFaces)!\n",
             nf);
    EG_free(iedge);
    EG_free(edges);
    EG_free(faces);
    return EGADS_MALLOC;
  }
  for (i = 0; i < nf; i++) facedg[i] = faces[iface[i]-1];

  /* deal with the edges touched */
  for (k = 0; k < nedge; k++)
    if (iedge[k] == 1) {
      /* adjust this edge here and now */
      stat = EG_retessEdge(tess, k+1, edges[k], params);
      if (stat != EGADS_SUCCESS) {
        if (outLevel > 0)
          printf(" EGADS Error: EG_retessEdge %d=%d (EG_retessFaces)!\n",
                 k+1, stat);
        EG_free(facedg);
        EG_free(iedge);
        EG_free(edges);
        EG_free(faces);
        return stat;
      }
    } else if (iedge[k] > 1) {
      /* hit multiple times -- defer */
      for (i = 0; i < nf; i++) {
        EG_getBodyTopos(object, faces[iface[i]-1], EDGE, &nef, &edgf);
        for (j = 0; j < nef; j++) {
          stat = EG_indexBodyTopo(object, edgf[j]);
          if (stat == k+1) facedg[i] = NULL;
        }
        EG_free(edgf);
      }
    }
  
  /* remove NULL faces and add the multiple hit edges */
  for (k = i = 0; i < nf; i++)
    if (facedg[i] != NULL) {
      facedg[k] = facedg[i];
      k++;
    }
  for (i = 0; i < nedge; i++)
    if (iedge[i] > 1) {
      facedg[k] = edges[i];
      k++;
    }
  EG_free(iedge);
  EG_free(edges);
  EG_free(faces);
  
  /* retessellate the faces (and twice+ hit edges) */
  stat = EG_remakeTess(tess, k, facedg, params);
  EG_free(facedg);
  return stat;
}

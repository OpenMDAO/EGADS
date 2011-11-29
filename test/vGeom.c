#include <math.h>
#include <string.h>

#include "egads.h"
#include "gv.h"


typedef struct {
  ego *faces;
  ego *faceTess;
  ego *edges;
  ego *edgeTess;
  ego body;
  int mtype;
  int nfaces;
  int nedges;
} bodyData;

  static int      nbody, modified = 0;
  static bodyData *bodydata;
  
  /* stuff from gv to connect with picking */
  extern int        gv_pickon, gv_pickout;
  extern GvGraphic *gv_picked;

  
  
int main(int argc, char *argv[])
{
  int    i, j, ibody, stat, nloops, nnodes, oclass, mtype, per;
  int    *senses, sizes[2], mtflag = 0;
  float  focus[4];
  double tlimits[4], box[6];
  ego    context, model, geom, *bodies, *loops, *nodes;

  if (argc != 2) {
    printf("\n Usage: vGeom filename\n\n");
    return 1;
  }

  /* initialize */
  printf(" EG_open           = %d\n", EG_open(&context));
  printf(" EG_loadModel      = %d\n", EG_loadModel(context, 0, argv[1], &model));
  printf(" EG_getBoundingBox = %d\n", EG_getBoundingBox(model, box));
  printf("       BoundingBox = %lf %lf %lf\n", box[0], box[1], box[2]);
  printf("                     %lf %lf %lf\n", box[3], box[4], box[5]);  
  printf(" \n");
  
  /* get all bodies */
  stat = EG_getTopology(model, &geom, &oclass, &mtype, NULL, &nbody,
                        &bodies, &senses);
  if (stat != EGADS_SUCCESS) {
    printf(" EG_getTopology = %d\n", stat);
    return 1;
  }
  printf(" EG_getTopology:     nBodies = %d\n", nbody);
  bodydata = (bodyData *) malloc(nbody*sizeof(bodyData));
  if (bodydata == NULL) {
    printf(" MALLOC Error on Body storage!\n");
    return 1;
  }
  
  /* fill our structure a body at at time */
  for (ibody = 0; ibody < nbody; ibody++) {
  
    mtype = 0;
    EG_getTopology(bodies[ibody], &geom, &oclass, 
                   &mtype, NULL, &j, &loops, &senses);
    bodydata[ibody].body  = bodies[ibody];
    bodydata[ibody].mtype = mtype;

    stat = EG_getBodyTopos(bodies[ibody], NULL, FACE, &bodydata[ibody].nfaces, 
                                                      &bodydata[ibody].faces);
    i    = EG_getBodyTopos(bodies[ibody], NULL, EDGE, &bodydata[ibody].nedges, 
                                                      &bodydata[ibody].edges);
    if ((stat != EGADS_SUCCESS) || (i != EGADS_SUCCESS)) {
      printf(" EG_getBodyTopos Face = %d\n", stat);
      printf(" EG_getBodyTopos Edge = %d\n", i);
      return 1;
    }

    printf(" EG_getBodyTopos:    %d nFaces  = %d\n", 
           ibody+1, bodydata[ibody].nfaces);
    printf(" EG_getBodyTopos:    %d nEdges  = %d\n", 
           ibody+1, bodydata[ibody].nedges);
    bodydata[ibody].faceTess = (ego *) 
                        malloc(bodydata[ibody].nfaces*sizeof(ego));
    bodydata[ibody].edgeTess = (ego *) 
                        malloc(bodydata[ibody].nedges*sizeof(ego));
    if ((bodydata[ibody].faceTess == NULL) || 
        (bodydata[ibody].edgeTess == NULL)) {
      printf(" CAN NOT Allocate Face Tessellation Objects!\n");
      return 1;
    }
    
    for (i = 0; i < bodydata[ibody].nfaces; i++) {
      bodydata[ibody].faceTess[i] = NULL;
      stat = EG_getTopology(bodydata[ibody].faces[i], &geom, &oclass, 
                            &mtype, NULL, &nloops, &loops, &senses);
      if (stat != EGADS_SUCCESS) continue;
      printf(" EG_getTopology:     %d Face %d -- nLoops = %d\n",
             ibody+1, i+1, nloops);
      stat = EG_getRange(bodydata[ibody].faces[i], tlimits, &per);
      if (stat != EGADS_SUCCESS) {
        printf(" EG_getRange Face return = %d!\n", stat);
        return 1;
      }
      sizes[0] = 37;
      sizes[1] = 37;
      if (mtype == SREVERSE) sizes[0] = -37;
      stat = EG_makeTessGeom(geom, tlimits, sizes,
                             &bodydata[ibody].faceTess[i]);
      if (stat != EGADS_SUCCESS) {
        printf(" EG_makeTessGeom Face return = %d!\n", stat);
        return 1;
      }
    }
    for (j = i = 0; i < bodydata[ibody].nedges; i++) {
      stat = EG_getTopology(bodydata[ibody].edges[i], &geom, &oclass, 
                            &mtype, NULL, &nnodes, &nodes, &senses);
      if (stat != EGADS_SUCCESS) continue;
      if (mtype == DEGENERATE) continue;
      printf(" EG_getTopology:     %d Edge %d -- nNodes = %d\n",
             ibody+1, i+1, nnodes);
      stat = EG_getRange(bodydata[ibody].edges[i], tlimits, &per);
      if (stat != EGADS_SUCCESS) {
        printf(" EG_getRange Edge return = %d!\n", stat);
        return 1;
      }
      sizes[0] = 37;
      sizes[1] = 0;               /* not required */
      stat = EG_makeTessGeom(geom, tlimits, sizes, 
                             &bodydata[ibody].edgeTess[j]);
      if (stat != EGADS_SUCCESS) {
        printf(" EG_makeTessGeom Edge return = %d!\n", stat);
        return 1;
      }
      bodydata[ibody].edges[j] = bodydata[ibody].edges[i];
      j++;
    }
    if (j != bodydata[ibody].nedges) {
      printf(" NOTE: %d Degenerate Edges removed!\n", 
             bodydata[ibody].nedges-j);
      bodydata[ibody].nedges = j;
    }
  }
  printf(" \n");

  focus[0] = 0.5*(box[0]+box[3]);
  focus[1] = 0.5*(box[1]+box[4]);
  focus[2] = 0.5*(box[2]+box[5]);
  focus[3] = sqrt( (box[0]-box[3])*(box[0]-box[3]) +
                   (box[1]-box[4])*(box[1]-box[4]) +
                   (box[2]-box[5])*(box[2]-box[5]) );

  gv_init("           View Geometry", mtflag, 0, NULL, NULL, NULL,
          NULL, focus);

  /* finish up */
  for (ibody = 0; ibody < nbody; ibody++) {
    for (i = 0; i < bodydata[ibody].nedges; i++) 
      EG_deleteObject(bodydata[ibody].edgeTess[i]);
    free(bodydata[ibody].edgeTess);
    for (i = 0; i < bodydata[ibody].nfaces; i++) 
      EG_deleteObject(bodydata[ibody].faceTess[i]);
    free(bodydata[ibody].faceTess);
    EG_free(bodydata[ibody].faces);
    EG_free(bodydata[ibody].edges);
  }
  free(bodydata);
  
  if (modified == 1)
    printf(" EG_saveModel      = %d\n", EG_saveModel(model, 
                                          "attrModel.egads"));

  printf(" EG_deleteObject   = %d\n", EG_deleteObject(model));
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
  int i, j, k, l, ibody, oclass, mtype, nloops, nledges, ng = 0;
  int *lsenses, *esenses;
  ego geom, topRef, prev, next, *loops, *ledges;
  
  for (ibody = 0; ibody < nbody; ibody++) {
    for (k = i = 0; i < bodydata[ibody].nfaces; i++) {
      EG_getTopology(bodydata[ibody].faces[i], &geom, &oclass, 
                     &mtype, NULL, &nloops, &loops, &lsenses);
/*    printf(" %d Face %d has %d loops\n", ibody+1, i+1, nloops);  */
      for (j = 0; j < nloops; j++) {
        EG_getTopology(loops[j], &geom, &oclass, &mtype, NULL, 
                       &nledges, &ledges, &esenses);
        for (l = 0; l < nledges; l++) {
          EG_getInfo(ledges[l], &oclass, &mtype, &topRef, 
                     &prev, &next);
          if (mtype != DEGENERATE) k++;
        }
      }
    }
    if (bodydata[ibody].mtype == WIREBODY) {
      EG_getTopology(bodydata[ibody].body, &geom, &oclass, 
                     &mtype, NULL, &nloops, &loops, &lsenses);
      for (j = 0; j < nloops; j++) {
        EG_getTopology(loops[j], &geom, &oclass, &mtype, NULL, 
                       &nledges, &ledges, &esenses);
        for (l = 0; l < nledges; l++) {
          EG_getInfo(ledges[l], &oclass, &mtype, &topRef, 
                     &prev, &next);
          if (mtype != DEGENERATE) k++;
        }
      }
    }
/*  printf(" %d: %d nonDegen Edges found!\n", ibody+1, k);  */
    ng += bodydata[ibody].nfaces+k;
  }
  gv_pickout = 1;       /* turns off gv output when picking is on */

  return ng;
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
  int        i, j, k, l, m, n, oclass, mtype, nloops, nledges, stat;
  int        ibody, ig, sizes[2], *lsenses, *esenses;
  char       title[16], etitl[16], ftitl[16];
  const char *astr;
  double     dcol, *xyzs, *ddata;
  ego        geom, *loops, *ledges;
  GvColor    color;
  GvObject   *object;

  dcol = 1.0/(nbody-1.0);
  for (ig = ibody = 0; ibody < nbody; ibody++) {
  
    /* get body name -- if exists */
    astr = NULL;
    EG_attributeRet(bodydata[ibody].body, "Name", &mtype, 
                    &l, NULL, NULL, &astr);
    if (astr == NULL) { 
      if (bodydata[ibody].mtype == WIREBODY) {
        sprintf(title, "WireBody #%d ", ibody+1);
      } else if (bodydata[ibody].mtype == FACEBODY) {
        sprintf(title, "FaceBody #%d ", ibody+1);
      } else if (bodydata[ibody].mtype == SHEETBODY) {
        sprintf(title, "SheetBody #%d ", ibody+1);
      } else {
        sprintf(title, "SolidBody #%d ", ibody+1);
      }    
    } else {
      for (j = 0; j < 15; j++) {
        title[j] = astr[j];
        if (title[j] == 0) break;
      }
      title[15] = 0;
    }
    if (bodydata[ibody].mtype == WIREBODY) gv_allocfamily(title);

    color.red   = 1.0;
    color.green = ibody*dcol;
    color.blue  = 0.0;

    for (i = 0; i < bodydata[ibody].nfaces; i++) {
    
      /* get face name */
      astr = NULL;
      EG_attributeRet(bodydata[ibody].faces[i], "Name", &mtype, 
                      &l, NULL, NULL, &astr);
      if (astr == NULL) { 
        sprintf(ftitl, "face #%d ", i+1);
      } else {
        for (j = 0; j < 15; j++) {
          ftitl[j] = astr[j];
          if (ftitl[j] == 0) break;
        }
        ftitl[15] = 0;
      }
      if (nbody == 1) {
        gv_allocfamily(ftitl);
      } else {
        gv_allocfamily(title);
      }

      stat = EG_getTessGeom(bodydata[ibody].faceTess[i], sizes, 
                            &xyzs);
      if (stat != EGADS_SUCCESS) {
        printf(" EG_getTessGeom Face %d = %d!\n", i+1, stat);
        return;
      }
      if (nbody == 1) {
        graphic[ig+i] = gv_alloc(GV_NONINDEXED, GV_QUADMESHS,
                                 GV_FOREGROUND|GV_ORIENTATION,
                                 color, "Surface         ", ibody, i+1);
      } else {
        graphic[ig+i] = gv_alloc(GV_NONINDEXED, GV_QUADMESHS,
                                 GV_FOREGROUND|GV_ORIENTATION,
                                 color, ftitl, ibody, i+1);
      }
      if (graphic[ig+i] == NULL) {
        printf("gv_alloc ERROR on graphic[%d]\n", i+1);
        return;
      }
      graphic[ig+i]->back.red   = 0.5;
      graphic[ig+i]->back.green = 0.5;
      graphic[ig+i]->back.blue  = 0.5;
      graphic[ig+i]->number     = 1;
      graphic[ig+i]->ddata      = xyzs;
      object = graphic[ig+i]->object;
      object->length = 1;
      object->type.qmeshes.size = (int *) malloc(2*sizeof(int));
      if (object->type.qmeshes.size == NULL) {
        printf("malloc ERROR on graphic[%d]\n", i+1);
        return;      
      }
      object->type.qmeshes.size[0] = sizes[0];
      object->type.qmeshes.size[1] = sizes[1];
      if (nbody == 1) {
        gv_adopt(ftitl, graphic[ig+i]);
      } else {
        gv_adopt(title, graphic[ig+i]);
      }
    }
  
    color.red   = 0.0;
    color.green = 0.0;
    color.blue  = 1.0;

    n = bodydata[ibody].nfaces + ig;
    for (i = 0; i < bodydata[ibody].nfaces; i++) {
    
      /* get the face name again */
      astr = NULL;
      EG_attributeRet(bodydata[ibody].faces[i], "Name", &mtype, 
                      &l, NULL, NULL, &astr);
      if (astr == NULL) { 
        sprintf(ftitl, "face #%d ", i+1);
      } else {
        for (j = 0; j < 15; j++) {
          ftitl[j] = astr[j];
          if (ftitl[j] == 0) break;
        }
        ftitl[15] = 0;
      }
      EG_getTopology(bodydata[ibody].faces[i], &geom, &oclass, 
                     &mtype, NULL, &nloops, &loops, &lsenses);
      for (j = 0; j < nloops; j++) {
        EG_getTopology(loops[j], &geom, &oclass, &mtype, NULL, &nledges,
                       &ledges, &esenses);
        for (k = 0; k < nledges; k++) {
          m = 0;
          while (ledges[k] != bodydata[ibody].edges[m]) {
            m++;
            if (m == bodydata[ibody].nedges) break;
          }
          /* assume that the edge is degenerate and removed */
          if (m == bodydata[ibody].nedges) continue;
          stat = EG_getTessGeom(bodydata[ibody].edgeTess[m], sizes, 
                                &xyzs);
          if (stat != EGADS_SUCCESS) {
            printf(" EG_getTessGeom Edge %d = %d!\n", m+1, stat);
            return;
          }
          astr = NULL;
          EG_attributeRet(bodydata[ibody].edges[m], "Name", &mtype, 
                          &l, NULL, NULL, &astr);
          if (astr == NULL) { 
            if (nbody == 1) {
              sprintf(etitl, "L%d E%d %d", j+1, m+1, esenses[k]);
            } else {
              sprintf(etitl, "F%d L%d E%d %d", i+1, j+1, m+1, esenses[k]);
            }
          } else {
            for (l = 0; l < 16; l++) etitl[l] = ' ';
            for (l = 0; l < 16; l++) {
              if (astr[l] == 0) break;
              etitl[l] = astr[l];
            }
          }
          graphic[n] = gv_alloc(GV_NONINDEXED, GV_POLYLINES, GV_FOREGROUND,
                                color, etitl, ibody, bodydata[ibody].nfaces+m+1);
          if (graphic[n] == NULL) {
            printf("gv_alloc ERROR on graphic[%d]\n", n+1);
            return;
          }
          ddata = xyzs;
          if (esenses[k] == -1) {
            ddata = (double *) malloc(3*sizes[0]*sizeof(double));
            /* note: needs to be checked and free'd later */
            for (m = 0; m < sizes[0]; m++) {
              l = sizes[0] - m - 1;
              ddata[3*l  ] = xyzs[3*m  ];
              ddata[3*l+1] = xyzs[3*m+1];
              ddata[3*l+2] = xyzs[3*m+2];
            }
          }
          graphic[n]->number = 1;
          object = graphic[n]->object;
          object->length = 1;
          object->type.plines.len    = (int *) malloc(sizeof(int));
          object->type.plines.len[0] = sizes[0];
          graphic[n]->ddata          = ddata;

          if (nbody == 1) {
            gv_adopt(ftitl, graphic[n]);
          } else {
            gv_adopt(title, graphic[n]);
          }
          n++;
        }
      }
    }
    
    /* put out Wires (i.e. no faces) */
    if (bodydata[ibody].mtype == WIREBODY) {
      EG_getTopology(bodydata[ibody].body, &geom, &oclass, 
                     &mtype, NULL, &nloops, &loops, &lsenses);
      for (j = 0; j < nloops; j++) {
        EG_getTopology(loops[j], &geom, &oclass, &mtype, NULL, &nledges,
                       &ledges, &esenses);
        for (k = 0; k < nledges; k++) {
          m = 0;
          while (ledges[k] != bodydata[ibody].edges[m]) {
            m++;
            if (m == bodydata[ibody].nedges) break;
          }
          /* assume that the edge is degenerate and removed */
          if (m == bodydata[ibody].nedges) continue;
          stat = EG_getTessGeom(bodydata[ibody].edgeTess[m], sizes, 
                                &xyzs);
          if (stat != EGADS_SUCCESS) {
            printf(" EG_getTessGeom Edge %d = %d!\n", m+1, stat);
            return;
          }
          astr = NULL;
          EG_attributeRet(bodydata[ibody].edges[m], "Name", &mtype, 
                          &l, NULL, NULL, &astr);
          if (astr == NULL) { 
            sprintf(etitl, "L%d E%d %d", j+1, m+1, esenses[k]);
          } else {
            for (l = 0; l < 16; l++) etitl[l] = ' ';
            for (l = 0; l < 16; l++) {
              if (astr[l] == 0) break;
              etitl[l] = astr[l];
            }
          }
          graphic[n] = gv_alloc(GV_NONINDEXED, GV_POLYLINES, GV_FOREGROUND,
                                color, etitl, ibody, m+1);
          if (graphic[n] == NULL) {
            printf("gv_alloc ERROR on graphic[%d]\n", n+1);
            return;
          }
          ddata = xyzs;
          if (esenses[k] == -1) {
            ddata = (double *) malloc(3*sizes[0]*sizeof(double));
            /* note: needs to be checked and free'd later */
            for (m = 0; m < sizes[0]; m++) {
              l = sizes[0] - m - 1;
              ddata[3*l  ] = xyzs[3*m  ];
              ddata[3*l+1] = xyzs[3*m+1];
              ddata[3*l+2] = xyzs[3*m+2];
            }
          }
          graphic[n]->number = 1;
          object = graphic[n]->object;
          object->length = 1;
          object->type.plines.len    = (int *) malloc(sizeof(int));
          object->type.plines.len[0] = sizes[0];
          graphic[n]->ddata          = ddata;
          gv_adopt(title, graphic[n]);
          n++;
        }
      }
    }
    
    ig = n;
  }

}


/* window defines */
#define DataBase        1
#define TwoD            2
#define ThreeD          3
#define Dials           4
#define Key             5

/* event types */
#define KeyPress        2
#define KeyRelease      3
#define ButtonPress     4
#define ButtonRelease   5
#define Expose          12
#define NoExpose        14
#define ClientMessage   33


void gvevent(int *win, int *type, /*@unused@*/ int *xpix, 
             /*@unused@*/ int *ypix, int *state)
{
  int          i, j, stat, ibody, eIndex, fIndex, nAttr, atype, len;
  const int    *ivec;
  char         chrs[100], chrss[100];
  const char   *name, *string;
  double       rval;
  const double *rvec;
  ego          obj;
  
  /* with picking on -- Button Press does attributes */
  if ((*win == ThreeD) && (*type == ButtonPress))
    if (gv_pickon == 1)
      if (gv_picked != NULL) {
        eIndex = fIndex = -1;
        ibody  = gv_picked->utype;
        if (gv_picked->uindex <= bodydata[ibody].nfaces) {
          fIndex = gv_picked->uindex - 1;
        } else {
          eIndex = gv_picked->uindex - bodydata[ibody].nfaces - 1;
        }
        printf(" Selected: ibody = %d, eIndex = %d, fIndex = %d\n", 
               ibody, eIndex, fIndex);
        if (fIndex == -1) {
          obj = bodydata[ibody].edges[eIndex];
        } else {
          obj = bodydata[ibody].faces[fIndex];
        }
        /* print the attributes */
        if (*state == 1) {
          nAttr = 0;
          EG_attributeNum(obj, &nAttr);
          for (i = 1; i <= nAttr; i++) {
            stat = EG_attributeGet(obj, i, &name, &atype, &len, &ivec,
                                   &rvec, &string);
            if (stat != EGADS_SUCCESS) continue;
            if (atype == ATTRINT) {
              printf(" Attr #%d: %s = %d", i, name, ivec[0]);
              for (j = 1; j < len; j++) printf(" %d", ivec[j]);
              printf("\n");
            } else if (atype == ATTRREAL) {
              printf(" Attr #%d: %s = %lf", i, name, rvec[0]);
              for (j = 1; j < len; j++) printf(" %lf", rvec[j]);
              printf("\n");
            } else {
              printf(" Attr #%d: %s = %s\n", i, name, string);
            }
          }
          printf("\n");
        }
        /* add an attribute */
        if (*state == 2) {
          printf(" Enter Attr Name and %d - %d for type: ", ATTRINT, ATTRSTRING);
          scanf("%s %d", chrs, &atype);
          if ((atype >= ATTRINT) && (atype <= ATTRSTRING)) {
            len = 1;
            printf(" Enter Value: ");
            if (atype == ATTRINT) {
              scanf("%d", &j);
            } else if (atype == ATTRREAL) {
              scanf("lf", &rval);
            } else {
              scanf("%s", chrss);
            }
            stat = EG_attributeAdd(obj, chrs, atype, len, &j, &rval, chrss);
            if (stat == EGADS_SUCCESS) {
              modified = 1;
            } else {
              printf("  attributeAdd fails with %d\n", stat);
            }
            printf("\n");
          }
        }
      }
}

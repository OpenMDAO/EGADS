#include <math.h>
#include <string.h>

#include "egads.h"
#include "gv.h"
#include "Graphics.h"

#define PRESENTATION		/* flip back & white definitions */


typedef struct {
  ego *faces;
  ego *edges;
  ego body;
  ego tess;
  int mtype;
  int nfaces;
  int nedges;
} bodyData;

  static int      nbody, modified = 0;
  static bodyData *bodydata;
  
  /* stuff from gv to connect with picking */
  extern int        gv_pickon, gv_pickout;
  extern GvGraphic *gv_picked;

#ifdef PRESENTATION
  extern float      gv_black[3], gv_white[3];
  extern GWinAtt    gv_wAux, gv_wDial;

  extern void       GraphicGCSetFB(GID gc, float *fg, float *bg);
  extern void       GraphicBGColor(GID win, float *bg);
#endif
  
  
int main(int argc, char *argv[])
{
  int          i, j, k, ibody, stat, oclass, mtype, len;
  int          *senses, mtflag = 0;
  float        focus[4], arg;
  double       box[6], size, t, params[3], dist, d;
  double       parms[3] = {0.0, 0.0, 0.0};
  const double *xyzs, *ts;
  ego          context, model, geom, *bodies, *dum;

  if ((argc != 2) && (argc != 5)) {
    printf("\n Usage: vTess filename [angle maxlen sag]\n\n");
    return 1;
  }

  /* look at EGADS revision */
  EG_revision(&i, &j);
  printf("\n Using EGADS %2d.%2d\n\n", i, j);

  /* initialize */
  printf(" EG_open           = %d\n", EG_open(&context));
  printf(" EG_loadModel      = %d\n", EG_loadModel(context, 0, argv[1], &model));
  printf(" EG_getBoundingBox = %d\n", EG_getBoundingBox(model, box));
  printf("       BoundingBox = %lf %lf %lf\n", box[0], box[1], box[2]);
  printf("                     %lf %lf %lf\n", box[3], box[4], box[5]);  
  printf(" \n");
  
  size = sqrt((box[0]-box[3])*(box[0]-box[3]) + (box[1]-box[4])*(box[1]-box[4]) +
              (box[2]-box[5])*(box[2]-box[5]));

  focus[0] = 0.5*(box[0]+box[3]);
  focus[1] = 0.5*(box[1]+box[4]);
  focus[2] = 0.5*(box[2]+box[5]);
  focus[3] = size;
  
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

  EG_setOutLevel(context, 2);		/* get some Debug output */
  params[0] =  0.025*size;
  params[1] =  0.001*size;
  params[2] = 15.0;
  if (argc == 5) {
    sscanf(argv[2], "%f", &arg);
    params[2] = arg;
    sscanf(argv[3], "%f", &arg);
    params[0] = arg;
    sscanf(argv[4], "%f", &arg);
    params[1] = arg;
    printf(" Using angle = %lf,  relSide = %lf,  relSag = %lf\n",
           params[2], params[0], params[1]);
    params[0] *= size;
    params[1] *= size;
  }
  
  /* fill our structure a body at at time */
  for (ibody = 0; ibody < nbody; ibody++) {
  
    mtype = 0;
    EG_getTopology(bodies[ibody], &geom, &oclass, 
                   &mtype, NULL, &j, &dum, &senses);
    bodydata[ibody].body  = bodies[ibody];
    bodydata[ibody].mtype = mtype;
    if (mtype == WIREBODY) {
      printf(" Body %d: Type = WireBody\n", ibody+1);
    } else if (mtype == FACEBODY) {
      printf(" Body %d: Type = FaceBody\n", ibody+1);
    } else if (mtype == SHEETBODY) {
      printf(" Body %d: Type = SheetBody\n", ibody+1);
    } else {
      printf(" Body %d: Type = SolidBody\n", ibody+1);
    }

    stat = EG_getBodyTopos(bodies[ibody], NULL, FACE, 
                           &bodydata[ibody].nfaces, &bodydata[ibody].faces);
    i    = EG_getBodyTopos(bodies[ibody], NULL, EDGE, 
                           &bodydata[ibody].nedges, &bodydata[ibody].edges);
    if ((stat != EGADS_SUCCESS) || (i != EGADS_SUCCESS)) {
      printf(" EG_getBodyTopos Face = %d\n", stat);
      printf(" EG_getBodyTopos Edge = %d\n", i);
      return 1;
    }
    
    stat = EG_makeTessBody(bodies[ibody], params, &bodydata[ibody].tess);
    if (stat != EGADS_SUCCESS) {
      printf(" EG_makeTessBody %d = %d\n", ibody, stat);
      continue;
    }
#ifdef REMAKE
    params[0] *= 0.5;
    stat = EG_remakeTess(bodydata[ibody].tess, 1, bodydata[ibody].edges,
                         params);
    printf(" EG_remakeTess = %d\n", stat);
    params[0] *= 2.0;
#endif

    /* split any Edges? */
    do {
      printf("\n Split Edge on Body %d [1-%d]/0 done: ",
             ibody+1, bodydata[ibody].nedges);
      scanf("%d", &i);
      if (i > 0) {
        EG_getTessEdge(bodydata[ibody].tess, i, &len, &xyzs, &ts);
        dist = 0.0;
        for (j = 0; j < len-1; j++) {
          d = (xyzs[3*j  ]-xyzs[3*j+3])*(xyzs[3*j  ]-xyzs[3*j+3]) +
              (xyzs[3*j+1]-xyzs[3*j+4])*(xyzs[3*j+1]-xyzs[3*j+4]) +
              (xyzs[3*j+2]-xyzs[3*j+5])*(xyzs[3*j+2]-xyzs[3*j+5]);
          if (d > dist) {
            dist = d;
            k    = j;
          }
        }
        t = 0.5*(ts[k] + ts[k+1]);
        stat = EG_insertEdgeVerts(bodydata[ibody].tess, i, k+1, 1, &t);
        if (stat != EGADS_SUCCESS)
          printf(" EG_insertEdgeVerts = %d\n", stat);
      }
    } while (i > 0);
    printf("\n");
    
    for (i = 1; i <= bodydata[ibody].nfaces; i++) {
      stat = EG_makeQuads(bodydata[ibody].tess, parms, i);
      if (stat == EGADS_SUCCESS)
        printf(" Quads available on Body = %d   Face = %d\n", ibody+1, i);
    }
  }
  printf(" \n");

#ifdef PRESENTATION
  gv_black[0] = gv_black[1] = gv_black[2] = 1.0;
  gv_white[0] = gv_white[1] = gv_white[2] = 0.0;
#endif
  gv_init("           View Topology", mtflag, 0, NULL, NULL, NULL,
          NULL, focus);

  /* finish up */
  for (ibody = 0; ibody < nbody; ibody++) {
    EG_deleteObject(bodydata[ibody].tess);
    EG_free(bodydata[ibody].edges);
    EG_free(bodydata[ibody].faces);
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
  int i, j, k, ibody, oclass, mtype, nloops, nledges, nq, ng = 0;
  int *lsenses, *esenses, *find;
  ego geom, *loops, *ledges;
  
  for (ibody = 0; ibody < nbody; ibody++)
    if (bodydata[ibody].mtype != WIREBODY) {
      for (k = i = 0; i < bodydata[ibody].nfaces; i++) {
        EG_getTopology(bodydata[ibody].faces[i], &geom, &oclass, 
                       &mtype, NULL, &nloops, &loops, &lsenses);
        for (j = 0; j < nloops; j++) {
          EG_getTopology(loops[j], &geom, &oclass, &mtype, NULL, 
                         &nledges, &ledges, &esenses);
          k += nledges;
        }
      }
      nq   = 0;
      find = NULL;
      EG_getTessQuads(bodydata[ibody].tess, &nq, &find);
      if (find != NULL) EG_free(find);
      ng += bodydata[ibody].nfaces+k+nq;
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
  int          i, j, k, l, m, n, oclass, mtype, nloops, nledges, stat;
  int          ibody, ig, nq, len, ntri, nu, nv, *lsenses, *esenses, *find;
  const int    *tris, *tric, *ptype, *pindex;
  char         title[16], etitl[16], ftitl[16];
  const char   *astr;
  double       dcol, *ddata;
  const double *xyzs, *uvs, *t;
  ego          geom, *loops, *ledges;
  GvColor      color;
  GvObject     *object;

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

      stat = EG_getTessFace(bodydata[ibody].tess, i+1, &len,
                            &xyzs, &uvs, &ptype, &pindex, &ntri,
                            &tris, &tric);
      if ((stat != EGADS_SUCCESS) && (stat != EGADS_NODATA)) {
        printf(" EG_getTessFace %d = %d!\n", i+1, stat);
        return;
      }
      if (nbody == 1) {
        graphic[ig+i] = gv_alloc(GV_INDEXED, GV_DISJOINTTRIANGLES,
                                 GV_FOREGROUND|GV_ORIENTATION,
                                 color, "Surface         ", ibody, i+1);
      } else {
        graphic[ig+i] = gv_alloc(GV_INDEXED, GV_DISJOINTTRIANGLES,
                                 GV_FOREGROUND|GV_ORIENTATION,
                                 color, ftitl, ibody, i+1);
      }
      if (graphic[ig+i] == NULL) {
        printf("gv_alloc ERROR on graphic[%d]\n", i+1);
        return;
      }
      graphic[ig+i]->back.red    = 0.5;
      graphic[ig+i]->back.green  = 0.5;
      graphic[ig+i]->back.blue   = 0.5;
#ifdef PRESENTATION
      graphic[ig+i]->mesh.red    = 0.0;
      graphic[ig+i]->mesh.green  = 0.0;
      graphic[ig+i]->mesh.blue   = 0.0;
#endif
      graphic[ig+i]->number      = 1;
      graphic[ig+i]->ddata       = (double *) xyzs;
      object = graphic[ig+i]->object;
      object->length             = ntri;
      object->type.distris.index = NULL;
      if (ntri != 0) {
        object->type.distris.index = (int *) malloc(3*ntri*sizeof(int));
        if (object->type.distris.index == NULL) {
          printf("malloc ERROR on graphic[%d]\n", i+1);
          return;      
        }
        for (j = 0; j < 3*ntri; j++)
          object->type.distris.index[j] = tris[j]-1;
      }
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
          stat = EG_getTessEdge(bodydata[ibody].tess, m+1, &len,
                                &xyzs, &t);
          if (stat != EGADS_SUCCESS) {
            printf(" EG_getTessEdge %d = %d!\n", m+1, stat);
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
          graphic[n] = gv_alloc(GV_NONINDEXED, GV_POLYLINES,
                                GV_FOREGROUND|GV_FORWARD,
                                color, etitl, ibody, bodydata[ibody].nfaces+m+1);
          if (graphic[n] == NULL) {
            printf("gv_alloc ERROR on graphic[%d]\n", n+1);
            return;
          }
          ddata = (double *) xyzs;
          if (esenses[k] == -1) {
            ddata = (double *) malloc(3*len*sizeof(double));
            if (ddata == NULL) {
              printf(" Cant allocate %d coordinates!\n", len);
              return;
            }
            /* note: needs to be checked and free'd later */
            for (m = 0; m < len; m++) {
              l = len - m - 1;
              ddata[3*l  ] = xyzs[3*m  ];
              ddata[3*l+1] = xyzs[3*m+1];
              ddata[3*l+2] = xyzs[3*m+2];
            }
          }
          graphic[n]->number = 1;
          object = graphic[n]->object;
          object->length = 1;
          object->type.plines.len = (int *) malloc(sizeof(int));
          if (object->type.plines.len != NULL) 
            object->type.plines.len[0] = len;
          graphic[n]->ddata = ddata;

          if (nbody == 1) {
            gv_adopt(ftitl, graphic[n]);
          } else {
            gv_adopt(title, graphic[n]);
          }
          n++;
        }
      }
    }
    
    color.red   = 0.0;
    color.green = 1.0;
    color.blue  = ibody*dcol;
    nq   = 0;
    find = NULL;
    EG_getTessQuads(bodydata[ibody].tess, &nq, &find);
    for (m = 0; m < nq; m++) {
      i = find[m]-1;

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
      if (nbody == 1) {
        graphic[n] = gv_alloc(GV_INDEXED, GV_QUADMESHS,
                              GV_FOREGROUND|GV_ORIENTATION,
                              color, "Quads           ", ibody, i+1);
      } else {
        graphic[n] = gv_alloc(GV_INDEXED, GV_QUADMESHS,
                              GV_FOREGROUND|GV_ORIENTATION,
                              color, ftitl, ibody, i+1);
      }
      if (graphic[n] == NULL) {
        printf("gv_alloc ERROR on graphic[%d]\n", n+1);
        return;
      }
      EG_getQuads(bodydata[ibody].tess, i+1, &len,
                  &xyzs, &uvs, &ptype, &pindex, &ntri);
      for (k = j = 0; j < ntri; j++) {
        EG_getPatch(bodydata[ibody].tess, i+1, j+1, 
                    &nu, &nv, &tris, &tric);
        k += nu*nv;
      }
      graphic[n]->back.red   = 0.5;
      graphic[n]->back.green = 0.5;
      graphic[n]->back.blue  = 0.5;
#ifdef PRESENTATION
      graphic[n]->mesh.red   = 0.0;
      graphic[n]->mesh.green = 0.0;
      graphic[n]->mesh.blue  = 0.0;
#endif
      graphic[n]->number     = 1;
      graphic[n]->ddata      = (double *) xyzs;
      object = graphic[n]->object;
      object->length = ntri;
      object->type.qmeshes.size  = (int *) malloc(2*ntri*sizeof(int));
      object->type.qmeshes.index = (int *) malloc(k*sizeof(int));
      for (k = j = 0; j < ntri; j++) {
        EG_getPatch(bodydata[ibody].tess, i+1, j+1, &nu, &nv, 
                    &tris, &tric);
        object->type.qmeshes.size[2*j  ] = nu;
        object->type.qmeshes.size[2*j+1] = nv;
        for (l = 0; l < nu*nv; l++, k++)
          object->type.qmeshes.index[k] = tris[l]-1;
      }
      if (nbody == 1) {
        gv_adopt(ftitl, graphic[n]);
      } else {
        gv_adopt(title, graphic[n]);
      }
      n++;
    }
    if (find != NULL) EG_free(find);        
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
#ifdef PRESENTATION
  static int   first = 0;

  if (first == 0) {
    GraphicGCSetFB(gv_wAux.GCs,  gv_white, gv_black);
    GraphicBGColor(gv_wAux.wid,            gv_black);
    GraphicGCSetFB(gv_wDial.GCs, gv_white, gv_black);
    GraphicBGColor(gv_wDial.wid,           gv_black);
    first++;
  }
#endif
  
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

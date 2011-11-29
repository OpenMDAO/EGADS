/*
 *      EGADS: Electronic Geometry Aircraft Design System
 *
 *             FORTRAN Bindings for GV
 *
 *      Copyright 2011, Massachusetts Institute of Technology
 *      Licensed under The GNU Lesser General Public License, version 2.1
 *      See http://www.opensource.org/licenses/lgpl-2.1.php
 *
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "egadsTypes.h"
#include "egadsInternals.h"

#include "gv.h"


  extern /*@null@*/ char *EG_f2c(const char *name, int nameLen);
  extern void EG_c2f(/*@null@*/ const char *string, char *name, int nameLen);
  
  /* call-backs */

#ifdef WIN32
  extern int  FGVUPDATE();
  extern void FGVDATA(int *ngraphic, int *graphics);
  extern int  FGVSCALAR(int *key, int *gohi, int *len, float *scalar);
#else
  extern int  fgvupdate_();
  extern void fgvdata_(int *ngraphic, int *graphics);
  extern int  fgvscalar_(int *key, int *gohi, int *len, float *scalar);
#endif

  /* graphic object handle data */

  static int        ngoh = 0;
  static GvGraphic **goh = NULL;


int
#ifdef WIN32
FGV_INIT (char *title, int *mt_flag, int *nscalar, int *keys, int *types, 
          double *lims, char *snam, double *focus, int titleLen, int snamLen)
#else
fgv_init_(char *title, int *mt_flag, int *nscalar, int *keys, int *types, 
          double *lims, char *snam, double *focus, int titleLen, int snamLen)
#endif
{
  int   i, j, k, stat;
  char  *ftitle, *fstr;
  float *flims;

  fstr   = NULL;
  if (*nscalar > 0) {
    fstr = (char *) EG_alloc(*nscalar*16*sizeof(char));
   if (fstr == NULL) return -1;
   for (i = 0; i < *nscalar*16; i++) fstr[i] = ' ';
   if (snamLen >= 16) {
     for (j = i = 0; i < *nscalar; i++)
       for (k = 0; k < 16; k++, j++)
         fstr[j] = snam[i*snamLen+k];
   } else {
     for (j = i = 0; i < *nscalar; i++)
       for (k = 0; k < snamLen; k++, j++)
         fstr[i*16+k] = snam[j];
   }
  }
  ftitle = EG_f2c(title, titleLen);
  flims  = (float *) EG_alloc((*nscalar*2+4)*sizeof(float));
  if (flims == NULL) {
    EG_free(ftitle);
    if (fstr != NULL) EG_free(fstr);
    return -1;
  }
  for (j = i = 0; i < *nscalar; i++, j += 2) {
    flims[j  ] = lims[2*i  ];
    flims[j+1] = lims[2*i+1];
  }
  flims[j  ] = focus[0];
  flims[j+1] = focus[1];
  flims[j+2] = focus[2];
  flims[j+3] = focus[3];
  stat = gv_init(ftitle, *mt_flag, *nscalar, keys, types, flims, 
                 fstr, &flims[j]);
  EG_free(flims);
  EG_free(ftitle);
  if (fstr != NULL) EG_free(fstr);
  return stat;
}


int
#ifdef WIN32
FGV_CREATE (int *gtype, int *attribute, float *fcolor, float *bcolor,
            char *title, int *len, int *sizes, int *indices, double *ddata, 
            int *utype, int *uindex, int titleLen)
#else
fgv_create_(int *gtype, int *attribute, float *fcolor, float *bcolor, 
            char *title, int *len, int *sizes, int *indices, double *ddata, 
            int *utype, int *uindex, int titleLen)
#endif
{
  int       i, j, k, index, indexed;
  char      *ftitle;
  GvGraphic *graphic, **tmp;
  GvObject  *object;
  GvColor   color;

  color.red   = fcolor[0];
  color.green = fcolor[1];
  color.blue  = fcolor[2];
  ftitle      = EG_f2c(title, titleLen);
  indexed     = GV_INDEXED;
  if (indices[0] == GV_NONINDEXED) indexed = indices[0];
  graphic     = gv_alloc(indexed, *gtype, *attribute, color, ftitle, *utype, *uindex);
  EG_free(ftitle);
  if (graphic == NULL) return -1;
  
  color.red       = bcolor[0];
  color.green     = bcolor[1];
  color.blue      = bcolor[2];
  graphic->back   = color;
  graphic->number = 1;
  graphic->ddata  = ddata;
  
  /* examine type and fill accordingly */
  
  object = graphic->object;
  object->length = *len;
  switch (*gtype) {
  
    case GV_POINTS:
      if (indexed == GV_INDEXED) {
        object->type.points.index = (int *) EG_alloc(*len*sizeof(int));
        if (object->type.points.index == NULL) {
          gv_free(graphic, 3);
          return -2;
        }
        for (i = 0; i < *len; i++)
          object->type.points.index[i] = indices[i] - 1;
      }

      break;
      
    case GV_DISJOINTLINES:
      if (indexed == GV_INDEXED) {
        object->type.dislines.index = (int *) EG_alloc(*len*2*sizeof(int));
        if (object->type.dislines.index == NULL) {
          gv_free(graphic, 3);
          return -2;
        }
        for (i = 0; i < *len*2; i++)
          object->type.dislines.index[i] = indices[i] - 1;
      }
      break;
      
    case GV_POLYLINES:
      object->type.plines.len = (int *) EG_alloc(*len*sizeof(int));
      if (object->type.plines.len == NULL) {
        gv_free(graphic, 3);
        return -2;
      }
      for (k = i = 0; i < *len; i++) {
        object->type.plines.len[i] = sizes[i];
        k += sizes[i];
      }
      if (indexed == GV_INDEXED) {
        object->type.plines.index = (int *) EG_alloc(k*sizeof(int));
        if (object->type.plines.index == NULL) {
          gv_free(graphic, 3);
          return -2;
        }
        for (j = i = 0; i < *len; i++)
          for (k = 0; k < sizes[i]; k++, j++)
            object->type.plines.index[j] = indices[j] - 1;
      }      
      break;
      
    case GV_DISJOINTTRIANGLES:
      if (indexed == GV_INDEXED) {
        object->type.distris.index = (int *) EG_alloc(*len*3*sizeof(int));
        if (object->type.distris.index == NULL) {
          gv_free(graphic, 3);
          return -2;
        }
        for (i = 0; i < *len*3; i++)
          object->type.distris.index[i] = indices[i] - 1;
      }
      break;
     
    case GV_POLYTRIANGLES:
      object->type.ptris.len = (int *) EG_alloc(*len*sizeof(int));
      if (object->type.ptris.len == NULL) {
        gv_free(graphic, 3);
        return -2;
      }
      for (k = i = 0; i < *len; i++) {
        object->type.ptris.len[i] = sizes[i];
        k += sizes[i];
      }
      if (indexed == GV_INDEXED) {
        object->type.ptris.index = (int *) EG_alloc(k*sizeof(int));
        if (object->type.ptris.index == NULL) {
          gv_free(graphic, 3);
          return -2;
        }
        for (j = i = 0; i < *len; i++)
          for (k = 0; k < sizes[i]; k++, j++)
            object->type.ptris.index[j] = indices[j] - 1;
      }      
      break;
    
    case GV_DISJOINTQUADS:
      if (indexed == GV_INDEXED) {
        object->type.disquads.index = (int *) EG_alloc(*len*4*sizeof(int));
        if (object->type.disquads.index == NULL) {
          gv_free(graphic, 3);
          return -2;
        }
        for (i = 0; i < *len*4; i++)
          object->type.disquads.index[i] = indices[i] - 1;
      }
      break;
      
    case GV_QUADMESHS:
      object->type.qmeshes.size = (int *) EG_alloc(*len*2*sizeof(int));
      if (object->type.qmeshes.size == NULL) {
        gv_free(graphic, 3);
        return -2;
      }
      for (k = i = 0; i < *len*2; i+=2) {
        object->type.qmeshes.size[i  ] = sizes[i  ];
        object->type.qmeshes.size[i+1] = sizes[i+1];
        k += sizes[i]*sizes[i+1];
      }
      if (indexed == GV_INDEXED) {
        object->type.qmeshes.index = (int *) EG_alloc(k*sizeof(int));
        if (object->type.qmeshes.index == NULL) {
          gv_free(graphic, 3);
          return -2;
        }
        for (j = i = 0; i < *len; i+=2)
          for (k = 0; k < sizes[i]*sizes[i+1]; k++, j++)
            object->type.qmeshes.index[j] = indices[j] - 1;
      }      
      break;
      
    case GV_TETRAHEDRA:
      if (indexed == GV_INDEXED) {
        object->type.tets.index = (int *) EG_alloc(*len*4*sizeof(int));
        if (object->type.tets.index == NULL) {
          gv_free(graphic, 3);
          return -2;
        }
        for (i = 0; i < *len*4; i++)
          object->type.tets.index[i] = indices[i] - 1;
      }
      break;
      
    case GV_PYRAMIDS:
      if (indexed == GV_INDEXED) {
        object->type.tets.index = (int *) EG_alloc(*len*5*sizeof(int));
        if (object->type.pyramids.index == NULL) {
          gv_free(graphic, 3);
          return -2;
        }
        for (i = 0; i < *len*5; i++)
          object->type.pyramids.index[i] = indices[i] - 1;
      }
      break;
      
    case GV_PRISMS:
      if (indexed == GV_INDEXED) {
        object->type.prisms.index = (int *) EG_alloc(*len*6*sizeof(int));
        if (object->type.prisms.index == NULL) {
          gv_free(graphic, 3);
          return -2;
        }
        for (i = 0; i < *len*6; i++)
          object->type.prisms.index[i] = indices[i] - 1;
      }
      break;
      
    case GV_HEXAHEDRA:
      if (indexed == GV_INDEXED) {
        object->type.hexas.index = (int *) EG_alloc(*len*8*sizeof(int));
        if (object->type.hexas.index == NULL) {
          gv_free(graphic, 3);
          return -2;
        }
        for (i = 0; i < *len*8; i++)
          object->type.hexas.index[i] = indices[i] - 1;
      }
      break;
      
    case GV_POLYTETRAHEDRA:
      object->type.ptets.len = (int *) EG_alloc(*len*sizeof(int));
      if (object->type.ptets.len == NULL) {
        gv_free(graphic, 3);
        return -2;
      }
      for (k = i = 0; i < *len; i++) {
        object->type.ptris.len[i] = sizes[i];
        k += sizes[i];
      }
      if (indexed == GV_INDEXED) {
        object->type.ptets.index = (int *) EG_alloc(k*sizeof(int));
        if (object->type.ptets.index == NULL) {
          gv_free(graphic, 3);
          return -2;
        }
        for (j = i = 0; i < *len; i++)
          for (k = 0; k < sizes[i]; k++, j++)
            object->type.ptets.index[j] = indices[j] - 1;
      }      

      break;
      
    case GV_BLOCKS:
      object->type.blocks.size = (int *) EG_alloc(*len*3*sizeof(int));
      if (object->type.blocks.size == NULL) {
        gv_free(graphic, 3);
        return -2;
      }
      for (k = i = 0; i < *len*3; i+=3) {
        object->type.blocks.size[i  ] = sizes[i  ];
        object->type.blocks.size[i+1] = sizes[i+1];
        object->type.blocks.size[i+2] = sizes[i+2];
        k += sizes[i]*sizes[i+1]*sizes[i+2];
      }
      if (indexed == GV_INDEXED) {
        object->type.qmeshes.index = (int *) EG_alloc(k*sizeof(int));
        if (object->type.qmeshes.index == NULL) {
          gv_free(graphic, 3);
          return -2;
        }
        for (j = i = 0; i < *len; i+=3)
          for (k = 0; k < sizes[i]*sizes[i+1]*sizes[i+2]; k++, j++)
            object->type.qmeshes.index[j] = indices[j] - 1;
      }      
      break;
  }
  
  /* find the slot and store away*/
  index = -1;
  if (ngoh > 0)
    for (i = 0; i < ngoh; i++)
      if (goh[i] == NULL) {
        index = i+1;
        break;
      }
  if (index == -1) {
    if (goh == NULL) {
      goh = (GvGraphic **) EG_alloc(64*sizeof(GvGraphic *));
      if (goh == NULL) {
        gv_free(graphic, 3);
        return -2;
      }
      for (i = 0; i < 64; i++) goh[i] = NULL;
      ngoh = 64;
    } else {
      index = ngoh + 64;
      tmp   = (GvGraphic **) EG_reall(goh, 64*sizeof(GvGraphic *));
      if (tmp == NULL) {
        gv_free(graphic, 3);
        return -2;
      }
      for (i = ngoh; i < index; i++) goh[i] = NULL;
      ngoh  = index;
      index = -1;
    }
  }
  if (index == -1) 
    for (index = i = 0; i < ngoh; i++)
      if (goh[i] == NULL) {
        index  = i+1;
        break;
      }
  if (index == 0) {
    gv_free(graphic, 3);
    return -3;    
  }
  
  goh[index-1] = graphic; 
  return index;
}


int
#ifdef WIN32
FGV_FREE (int *index)
#else
fgv_free_(int *index)
#endif
{
  if ((*index < 1) || (*index > ngoh)) return -3;
  if (goh[*index-1] == NULL) return -4;

  gv_free(goh[*index-1], 3);
  goh[*index-1] = NULL;
  return 0;
}


int
#ifdef WIN32
FGV_HANDSHAKE (int *hold, int *ngraphics)
#else
fgv_handshake_(int *hold, int *ngraphics)
#endif
{
  return gv_handshake(*hold, *ngraphics);
}


int
#ifdef WIN32
FGV_STATUS (int *ngraphics)
#else
fgv_status_(int *ngraphics)
#endif
{
  return gv_status(ngraphics);
}


int
#ifdef WIN32
FGV_LOCATE (double *xyz)
#else
fgv_locate_(double *xyz)
#endif
{
  return gv_locate(xyz);
}


int
#ifdef WIN32
FGV_CATCH (char *str, int strLen)
#else
fgv_catch_(char *str, int strLen)
#endif
{
  int  stat;
  char *string;
  
  stat = gv_catch(&string);
  EG_c2f(string, str, strLen);
  if (string != NULL) {
    if (strlen(string) > strLen) stat = -2;
    EG_free(string);
  }
  return stat;
}


int
#ifdef WIN32
FGV_ALLOCFAMILY (char *str, int strLen)
#else
fgv_allocfamily_(char *str, int strLen)
#endif
{
  int  stat;
  char *string;
  
  string = EG_f2c(str, strLen);
  stat   = gv_allocfamily(string);
  EG_free(string);
  return stat;
}


int
#ifdef WIN32
FGV_FREEFAMILY (char *str, int strLen)
#else
fgv_freefamily_(char *str, int strLen)
#endif
{
  int  stat;
  char *string;
  
  string = EG_f2c(str, strLen);
  stat   = gv_freefamily(string);
  EG_free(string);
  return stat;
}


int
#ifdef WIN32
FGV_ADOPT (char *str, int *index, int strLen)
#else
fgv_adopt_(char *str, int *index, int strLen)
#endif
{
  int  stat;
  char *string;
  
  if ((*index < 1) || (*index > ngoh)) return -3;
  if (goh[*index-1] == NULL) return -4;

  string = EG_f2c(str, strLen);
  stat   = gv_adopt(string, goh[*index-1]);
  EG_free(string);
  return stat;
}


int
#ifdef WIN32
FGV_SETFAMILY (char *str, int *type, int *attr, int strLen)
#else
fgv_setfamily_(char *str, int *type, int *attr, int strLen)
#endif
{
  int  stat;
  char *string;
  
  string = EG_f2c(str, strLen);
  stat   = gv_setfamily(string, *type, *attr);
  EG_free(string);
  return stat;
}


int
#ifdef WIN32
FGV_GETFAMILY (char *str, int *type, int *attr, int strLen)
#else
fgv_getfamily_(char *str, int *type, int *attr, int strLen)
#endif
{
  int  stat;
  char *string;
  
  string = EG_f2c(str, strLen);
  stat   = gv_getfamily(string, *type, attr);
  EG_free(string);
  return stat;
}


int
#ifdef WIN32
FGV_NUMFAMILY ( )
#else
fgv_numfamily_( )
#endif
{
  return gv_numfamily();
}


int
#ifdef WIN32
FGV_RETURNFAMILY (int *index, char *str, int strLen)
#else
fgv_returnfamily_(int *index, char *str, int strLen)
#endif
{
  int  stat;
  char *string;
  
  stat = gv_returnfamily(*index-1, &string);
  EG_c2f(string, str, strLen);
  if (string != NULL)
    if (strlen(string) > strLen) stat = -2;
  return stat;
}


void
#ifdef WIN32
FGV_DUMP (int *index, char *str, int strLen)
#else
fgv_dump_(int *index, char *str, int strLen)
#endif
{
  char *string;
  
  string = EG_f2c(str, strLen);
  gv_dump(*index, string);
  EG_free(string);
}


void
#ifdef WIN32
FGV_CLEANUP ( )
#else
fgv_cleanup_( )
#endif
{
  int  i, n;
  char *family;
  
  n = gv_numfamily();
  for (i = n-1; i >= 0; i++) {
    gv_returnfamily(i, &family);
    gv_freefamily(family);
  }

  for (i = 0; i < ngoh; i++) {
    if (goh[i] == NULL) continue;
    gv_free(goh[i], 3);
  }

  EG_free(goh);
  goh  = NULL;
  ngoh = 0;
}


/* call back interface */


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
#ifdef WIN32
  return FGVUPDATE( );
#else
  return fgvupdate_( );
#endif
}


void gvdata( int ngraphics, GvGraphic *graphic[] )
/*
 *  used to (re)set the graphics objects to be used in plotting
 *
 *  ngraphics is the number of objects to be filled
 *                           (from gvupdate or gv_handshake)
 *
 */
{
  int i, j, *gohi;
  
  gohi = (int *) EG_alloc(ngraphics*sizeof(int));
  if (gohi == NULL) return;
  
  /* fill the handles with the current graphics */
  for (i = 0; i < ngraphics; i++) {
    gohi[i] = 0;
    for (j = 0; j < ngoh; j++)
      if (graphic[i] == goh[j]) {
        gohi[i] = j+1;
        break;
      }
  }
  
#ifdef WIN32
  FGVDATA (&ngraphics, gohi);
#else
  fgvdata_(&ngraphics, gohi);
#endif

  /* set the graphics from the updated handles */
  for (i = 0; i < ngraphics; i++) {
    graphic[i] = NULL;
    if (gohi[i] <= 0) continue;
    graphic[i] = goh[gohi[i]-1];
  }
  EG_free(gohi);
}


int
gvscalar( int key, GvGraphic *graphic, int len, float *scalar )
/*
 *  called to get scalar value for color rendering for graphics objects
 *
 *  key     - scalar index (from gv_init)
 *  graphic - the GvGraphic structure for scalar fill
 *  len     - len of scalar to be filled
 *  scalar  - scalar to be filled
 */
{
  int i, keyf, gohi;
   
  for (gohi = i = 0; i < ngoh; i++)
    if (graphic == goh[i]) {
      gohi = i+1;
      break;
    }
  if (gohi == 0) return 0;

  keyf = key+1;
#ifdef WIN32
  return FGVSCALAR (&keyf, &gohi, &len, scalar);
#else
  return fgvscalar_(&keyf, &gohi, &len, scalar);
#endif
}


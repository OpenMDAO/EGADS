#include "egads.h"


int main(/*@unused@*/int argc, char *argv[])
{
  int i, mtype, oclass, nbody, nbody1, nbody2, *senses;
  ego context, model1, model2, model, geom, *bodies1, *bodies2;
  ego *bodies;
  
  /* initialize */
  printf(" EG_open           = %d\n", EG_open(&context));
  printf(" EG_loadModel 1    = %d\n", EG_loadModel(context, 0, argv[1], &model1));
  printf(" EG_loadModel 2    = %d\n", EG_loadModel(context, 0, argv[2], &model2));
  /* get all bodies */
  printf(" EG_getTopology 1  = %d\n", EG_getTopology(model1, &geom, &oclass, 
                                                     &mtype, NULL, &nbody1,
                                                     &bodies1, &senses));
  printf(" EG_getTopology 2  = %d\n", EG_getTopology(model2, &geom, &oclass, 
                                                     &mtype, NULL, &nbody2,
                                                     &bodies2, &senses));
  nbody  = nbody1+nbody2;
  bodies = (ego *) malloc(nbody*sizeof(ego));
  if (bodies == NULL) {
    printf(" Malloc error on %d!\n\n", nbody);
    return 1;
  }
  printf(" \n");
  for (i = 0; i < nbody1; i++)
    EG_copyObject(bodies1[i], NULL, &bodies[i]);
  for (i = 0; i < nbody2; i++)
    EG_copyObject(bodies2[i], NULL, &bodies[i+nbody1]);
  printf(" EG_deleteObject   = %d\n", EG_deleteObject(model2));
  printf(" EG_deleteObject   = %d\n", EG_deleteObject(model1));
  printf(" EG_makeTopology   = %d\n", EG_makeTopology(context, NULL, MODEL, 0,
                                                     NULL, nbody, bodies, NULL,
                                                     &model));
  printf(" EG_saveModel      = %d\n", EG_saveModel(model, "agglom.egads"));

  printf(" EG_deleteObject   = %d\n", EG_deleteObject(model));
  
  free(bodies);
  printf(" EG_close          = %d\n", EG_close(context));
  return 0;
}

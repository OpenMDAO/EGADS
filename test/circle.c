#include "egads.h"


int main(/*@unused@*/int argc, char *argv[])
{
  int    i, stat, periodic;
  ego    context, circle;
  double t, range[2], vals[9];
/*
  double data[10] = {0.0, 10.0, -5.0,  2.0, 0.0, 0.0,
                     0.0, -2.0,  0.0,  2.0};
*/
  double data[10] = {0.0, 10.0, -5.0,  -2.0, 0.0, 0.0,
                     0.0,  0.0, -2.0,   2.0};
/*
  double data[10] = {-10.0, 0.0, -5.0,  0.0,  1.0,  0.0,
                       0.0, 0.0, -1.0,  2.0};
*/
  
  printf(" EG_open         = %d\n", EG_open(&context));
  printf(" EG_makeGeometry = %d\n", EG_makeGeometry(context, CURVE, CIRCLE,
                                                    NULL, NULL, data, &circle));
  printf(" EG_getRange     = %d\n", EG_getRange(circle, range, &periodic));
  printf("                   %lf %lf  per = %d\n", range[0],range[1], periodic);
/*
  {
    int    class, type, *info;
    double *data;
    ego    ref;

    stat = EG_getGeometry(circle, &class, &type, &ref, &info, &data);
    printf("  getGeom = %d   %d (%d)  %d (%d)\n",
           stat, class, CURVE, type, CIRCLE);
    printf("   %lf %lf %lf   %lf %lf %lf\n", data[0], data[1], data[2],
           data[3], data[4], data[5]);
    printf("   %lf %lf %lf   %lf\n", data[6], data[7], data[8], data[9]);
    EG_free(info);
    EG_free(data);
  }
*/
  for (i = 0; i < 5; i++) {
    t    = range[0] + 0.25*i*(range[1]-range[0]);
    stat = EG_evaluate(circle, &t, vals);
    printf(" EG_evaluate     = %d  %lf  %lf %lf %lf\n",
           stat, t, vals[0], vals[1], vals[2]);
  }
  printf(" EG_deleteObject = %d\n", EG_deleteObject(circle));
  printf(" EG_close        = %d\n", EG_close(context));
  return 0;
}

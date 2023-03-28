#ifndef MIDAS_R_H
#define MIDAS_R_H

#include "count_min_sketch.h"
#include "prepare.h"
#include <math.h>
#include <stdlib.h>

typedef struct {
  double factor;
  CountMinSketch numCurrentEdge;
  union {
    NitroSketch ns_numTotalEdge;
    CountMinSketch numTotalEdge;
  };
  CountMinSketch numCurrentSource;
  union {
    NitroSketch ns_numTotalSource;
    CountMinSketch numTotalSource;
  };

  CountMinSketch numCurrentDestination;
  union {
    NitroSketch ns_numTotalDestination;
    CountMinSketch numTotalDestination;
  };
  int current_ts;
} MidasR;

MidasR *nitro_midasRInit(int total_depth, int total_width, int current_depth,
                         int current_width, double factor, gsl_rng *r);
MidasR *midasRInit(int total_depth, int total_width, int current_depth,
                   int current_width, double factor);
double midasROperator(MidasR *midasR, Input input);
double nitro_midasROperator(MidasR *midasR, Input input);
void midasRFree(MidasR *midasR);
void midasR_Nitro_Free(MidasR *midasR);

#endif

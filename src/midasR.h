#ifndef MIDAS_R_H
#define MIDAS_R_H

#include "count_min_sketch.h"
#include "nitro_sketch.h"
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
  CountMinSketch numCurrentSrc;
  union {
    NitroSketch ns_numTotalSrc;
    CountMinSketch numTotalSrc;
  };

  CountMinSketch numCurrentDst;
  union {
    NitroSketch ns_numTotalDst;
    CountMinSketch numTotalDst;
  };
  int current_ts;
} MidasR;

MidasR *nitro_midasRInit(int total_depth, int total_width, int current_depth,
                         int current_width, double factor, gsl_rng *r);
MidasR *midasRInit(int total_depth, int total_width, int current_depth,
                   int current_width, double factor);
double midasROperator(MidasR *midasR, Input input);
double nitro_midasROperator(MidasR *midasR, Input input, double prob);
void midasRFree(MidasR *midasR);
void midasR_Nitro_Free(MidasR *midasR);

#endif

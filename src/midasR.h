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

MidasR *midasR_Plus_Init(int total_depth, int total_width, int current_depth,
                         int current_width, double factor, gsl_rng *r);
MidasR *midasR_Init(int total_depth, int total_width, int current_depth,
                   int current_width, double factor);
double midasR_Operator(MidasR *midasR, Input input);
double midasR_Plus_Operator(MidasR *midasR, Input input, double prob);
void midasR_Free(MidasR *midasR);
void midasR_Plus_Free(MidasR *midasR);

#endif

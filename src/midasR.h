#ifndef MIDAS_R_H
#define MIDAS_R_H

#include "count_min_sketch.h"
#include "prepare.h"
#include <stdlib.h>
#include <math.h>

typedef struct {
  double factor;
  CountMinSketch numCurrentEdge, numTotalEdge;
  CountMinSketch numCurrentSource, numTotalSource;
  CountMinSketch numCurrentDestination, numTotalDestination;
  int current_ts;
} MidasR;

MidasR *midasRInit(int depth, int width, double factor);
double midasROperator(MidasR *midasR, Input input);

#endif

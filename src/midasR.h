#ifndef MIDAS_R_H
#define MIDAS_R_H

#include "count_min_sketch.h"
#include "prepare.h"
#include <math.h>
#include <stdlib.h>

typedef struct {
  double factor;
  CountMinSketch numCurrentEdge, numTotalEdge;
  CountMinSketch numCurrentSource, numTotalSource;
  CountMinSketch numCurrentDestination, numTotalDestination;
  int current_ts;
} MidasR;

MidasR *midasRInit(int depth, int width, double factor);
double midasROperator(MidasR *midasR, Input input);
double new_midasROperator(MidasR *midasR, Input input);
void midasRFree(MidasR *midasR);

#endif

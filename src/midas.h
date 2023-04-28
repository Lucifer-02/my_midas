#ifndef MIDAS_H
#define MIDAS_H

#include "count_min_sketch.h"
#include "nitro_sketch.h"
#include "prepare.h"

#include <math.h>
#include <stdio.h>
#include <stdlib.h>

typedef struct {

  CountMinSketch current;
  union {
    CountMinSketch total;
    NitroSketch n_total;
  };

  int current_ts;
} Midas;

Midas *midas_Init(int total_depth, int total_width, int current_depth,
                  int current_width);
Midas *midas_Plus_Init(int total_depth, int total_width, int current_depth,
                       int current_width, gsl_rng *r);
double midas_Operator(Midas *midas, Input input);
double midas_Plus_Operator(Midas *midas, Input input, double prob);
void midas_Free(Midas *midas);
void midas_Plus_Free(Midas *midas);

#endif

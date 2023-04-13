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

Midas *midasInit(int total_depth, int total_width, int current_depth,
                 int current_width);
Midas *nitro_midasInit(int total_depth, int total_width, int current_depth,
                       int current_width, gsl_rng *r);
double midasOperator(Midas *midas, Input input);
double nitro_midasOperator(Midas *midas, Input input, double prob);
void midasFree(Midas *midas);
void nitro_midasFree(Midas *midas);

#endif

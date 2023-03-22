#ifndef MIDAS_H
#define MIDAS_H

#include "count_min_sketch.h"
#include "prepare.h"

#include <math.h>
#include <stdio.h>
#include <stdlib.h>

typedef struct {
  CountMinSketch current;
  CountMinSketch total;
  int current_ts;
} Midas;

Midas *midasInit(int total_depth, int total_width, int current_depth,
                 int current_width);
double midasOperator(Midas *midas, Input input);
double new_midasOperator(Midas *midas, Input input);
double geo_midasOperator(Midas *midas, Input input, gsl_rng *r, uint32_t *row);
void midasFree(Midas *midas);

#endif

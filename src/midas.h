#ifndef MIDAS_H 
#define MIDAS_H

#include "count_min_sketch.h"
#include "prepare.h"

#include <math.h>
#include <stdlib.h>
#include <stdio.h>

typedef struct {
  CountMinSketch current;
  CountMinSketch total;
  int current_ts;
} Midas;

Midas *midasInit(int depth, int width) ;
double midasOperator(Midas *midas, Input input) ;

#endif


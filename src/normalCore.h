#ifndef NORMAL_H
#define NORMAL_H

#include "count_min_sketch.h"
#include "config.h"
#include <math.h>
#include <stdlib.h>
#include <stdio.h>

typedef struct {
  CountMinSketch current;
  CountMinSketch total;
  int current_ts;
} NormalCore;

double ComputeScore(double a, double s, double t) ;
NormalCore *midasInit(Config config) ;
double normalOperator(NormalCore *midas, Input input) ;

#endif


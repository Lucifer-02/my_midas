#include "midas.h"
#include "count_min_sketch.h"
#include <stdio.h>

static double ComputeScore(double a, double s, double t) {
  return s == 0 || t - 1 == 0 ? 0 : pow((a - s / t) * t, 2) / (s * (t - 1));
}

Midas *midasInit(int depth, int width) {
  Midas *midas = malloc(sizeof(Midas));
  cms_init(&(midas->current), width, depth);
  cms_init(&(midas->total), width, depth);
  midas->current_ts = 1;

  // print error rate and confidence
  printf("MIDAS: Depth: %d, Width: %d, Error rate: %f, Confidence: %f\n", depth,
         width, midas->current.error_rate, midas->current.confidence);

  return midas;
}

double midasOperator(Midas *midas, Input input) {
  if (input.ts > midas->current_ts) {
    cms_clear(&(midas->current));
    midas->current_ts = input.ts;
  }

  char hash[100];
  sprintf(hash, "%d", input.src * 13 + input.dst * 17);
  cms_add(&(midas->current), hash);
  cms_add(&(midas->total), hash);

  return ComputeScore(cms_check(&(midas->current), hash),
                      cms_check(&(midas->total), hash), input.ts);
}

double new_midasOperator(Midas *midas, Input input) {
  if (input.ts > midas->current_ts) {
    cms_clear(&(midas->current));
    midas->current_ts = input.ts;
  }

  char hash[100];
  sprintf(hash, "%d", input.src * 13 + input.dst * 17);
  cms_add(&(midas->current), hash);
  my_add(&(midas->total), hash, 1.0, 0.25);

  return ComputeScore(cms_check(&(midas->current), hash),
                      cms_check_median(&(midas->total), hash), input.ts);
}

double geo_midasOperator(Midas *midas, Input input) {
  if (input.ts > midas->current_ts) {
    cms_clear(&(midas->current));
    midas->current_ts = input.ts;
  }

  char hash[100];
  sprintf(hash, "%d", input.src * 13 + input.dst * 17);
  cms_add(&(midas->current), hash);
  geo_add(&(midas->total), hash, 1.0, 0.25);

  return ComputeScore(cms_check(&(midas->current), hash),
                      cms_check_median(&(midas->total), hash), input.ts);
}
void midasFree(Midas *midas) {
  cms_destroy(&(midas->current));
  cms_destroy(&(midas->total));
  free(midas);
}

#include "midas.h"
#include "count_min_sketch.h"
#include <gsl/gsl_rng.h>
#include <stdio.h>

static double ComputeScore(double a, double s, double t) {
  return s == 0 || t - 1 == 0 ? 0 : pow((a - s / t) * t, 2) / (s * (t - 1));
}

Midas *midasInit(int total_depth, int total_width, int current_depth,
                 int current_width) {
  Midas *midas = malloc(sizeof(Midas));
  cms_init(&(midas->total), total_width, total_depth);
  cms_init(&(midas->current), current_width, current_depth);
  midas->current_ts = 1;

  return midas;
}

Midas *nitro_midasInit(int total_depth, int total_width, int current_depth,
                       int current_width, gsl_rng *r) {
  Midas *midas = malloc(sizeof(Midas));
  ns_init(&(midas->n_total), total_width, total_depth, r);
  cms_init(&(midas->current), current_width, current_depth);
  midas->current_ts = 1;

  return midas;
}

double midasOperator(Midas *midas, Input input) {

  if (input.ts > midas->current_ts) {
    cms_clear(&(midas->current));
    midas->current_ts = input.ts;
  }

  char key[32];
  sprintf(key, "%d", input.src * 13 + input.dst * 17);
  cms_add(&(midas->current), key);
  cms_add(&(midas->total), key);

  return ComputeScore(cms_add_check(&(midas->current), key, 1.0),
                      cms_add_check(&(midas->total), key, 1.0), input.ts);

  // return 1;
}

double nitro_midasOperator(Midas *midas, Input input) {

  if (input.ts > midas->current_ts) {
    cms_clear(&(midas->current));
    midas->current_ts = input.ts;
  }

  char key[32];
  sprintf(key, "%d", input.src * 13 + input.dst * 17);
  cms_add_fast(&(midas->current), key);
  ns_add(&(midas->n_total), key, 1.0, 0.125);

  return ComputeScore(cms_check_fast(&(midas->current), key),
                      ns_check_mean(&(midas->n_total), key), input.ts);

  // return 1;
}

void midasFree(Midas *midas) {
  cms_destroy(&(midas->current));
  cms_destroy(&(midas->total));
  free(midas);
}

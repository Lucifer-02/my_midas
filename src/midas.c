#include "midas.h"
#include "count_min_sketch.h"
#include "nitro_sketch.h"
#include <gsl/gsl_rng.h>

static double ComputeScore(double a, double s, double t) {
  return s == 0 || t - 1 == 0 ? 0 : pow((a - s / t) * t, 2) / (s * (t - 1));
}

static int combine(int a, int b) { return a * 17 + b * 13; }

Midas *midas_Init(int total_depth, int total_width, int current_depth,
                 int current_width) {
  Midas *midas = malloc(sizeof(Midas));
  cms_init(&(midas->total), total_width, total_depth);
  cms_init(&(midas->current), current_width, current_depth);
  midas->current_ts = 1;

  return midas;
}

Midas *midas_Plus_Init(int total_depth, int total_width, int current_depth,
                       int current_width, gsl_rng *r) {
  Midas *midas = malloc(sizeof(Midas));
  ns_init(&(midas->n_total), total_width, total_depth, r);
  cms_init(&(midas->current), current_width, current_depth);
  midas->current_ts = 1;

  return midas;
}

double midas_Operator(Midas *midas, Input input) {

  if (input.ts > midas->current_ts) {
    cms_clear(&(midas->current));
    midas->current_ts = input.ts;
  }

  char key[32];
  sprintf(key, "%d", combine(input.src, input.dst));
  cms_add(&(midas->current), key);
  cms_add(&(midas->total), key);

  return ComputeScore(cms_check(&(midas->current), key),
                      cms_check(&(midas->total), key), input.ts);

  // return 1;
}

double midas_Plus_Operator(Midas *midas, Input input, double prob) {

  if (input.ts > midas->current_ts) {
    cms_clear(&(midas->current));
    midas->current_ts = input.ts;
  }

  char key[32];
  sprintf(key, "%d", combine(input.src, input.dst));
  cms_add_fast(&(midas->current), key);
  ns_add(&(midas->n_total), key, 1.0, prob);

  return ComputeScore(cms_check_fast(&(midas->current)),
                      ns_check_median_fast(&(midas->n_total), key), input.ts);

  // return 1;
}

void midas_Free(Midas *midas) {
  cms_destroy(&(midas->current));
  cms_destroy(&(midas->total));
  free(midas);
}

void midas_Plus_Free(Midas *midas) {
  cms_destroy(&(midas->current));
  ns_destroy(&(midas->n_total));
  free(midas);
}

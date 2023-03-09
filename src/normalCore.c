#include "normalCore.h"
#include <stdio.h>

static double ComputeScore(double a, double s, double t) {
  return s == 0 || t - 1 == 0 ? 0 : pow((a - s / t) * t, 2) / (s * (t - 1));
}

NormalCore *midasInit(int depth, int width) {
  NormalCore *midas = malloc(sizeof(NormalCore));
  cms_init(&(midas->current), width, depth);
  cms_init(&(midas->total), width, depth);
  midas->current_ts = 0;

  // print error rate and confidence
  printf("Depth: %d, Width: %d, Error rate: %f, Confidence: %f\n", depth, width,
         midas->current.error_rate, midas->current.confidence);

  return midas;
}

double normalOperator(NormalCore *midas, Input input) {
  if (input.ts > midas->current_ts) {
    cms_clear(&(midas->current));
    midas->current_ts = input.ts;
  }

  char hash[100];
  sprintf(hash, "%d", input.src + input.dst);
  cms_add(&(midas->current), hash);
  cms_add(&(midas->total), hash);

  return ComputeScore(cms_check(&(midas->current), hash),
                      cms_check(&(midas->total), hash), input.ts);
}

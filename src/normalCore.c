#include "normalCore.h"

double ComputeScore(double a, double s, double t) {
  return s == 0 || t - 1 == 0 ? 0 : pow((a - s / t) * t, 2) / (s * (t - 1));
}

NormalCore *midasInit(Config config) {
  NormalCore *midas = malloc(sizeof(NormalCore));
  cms_init(&(midas->current), config.width, config.depth);
  cms_init(&(midas->total), config.width, config.depth);
  midas->current_ts = 0;

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

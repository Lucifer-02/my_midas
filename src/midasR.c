#include "midasR.h"
#include "count_min_sketch.h"
#include "nitro_sketch.h"

static int combine(int a, int b) { return a * 17 + b * 13; }

MidasR *midasR_Init(int total_depth, int total_width, int current_depth,
                    int current_width, double factor) {
  MidasR *midasR = malloc(sizeof(MidasR));
  midasR->factor = factor;

  cms_init(&(midasR->numCurrentEdge), current_width, current_depth);
  cms_init(&(midasR->numTotalEdge), total_width, total_depth);
  cms_init(&(midasR->numCurrentSrc), current_width, current_depth);
  cms_init(&(midasR->numTotalSrc), total_width, total_depth);
  cms_init(&(midasR->numCurrentDst), current_width, current_depth);
  cms_init(&(midasR->numTotalDst), total_width, total_depth);

  midasR->current_ts = 1;

  return midasR;
}

MidasR *midasR_Plus_Init(int total_depth, int total_width, int current_depth,
                         int current_width, double factor, gsl_rng *r) {

  MidasR *midasR = malloc(sizeof(MidasR));
  midasR->factor = factor;

  cms_init(&(midasR->numCurrentEdge), current_width, current_depth);
  ns_init(&(midasR->ns_numTotalEdge), total_width, total_depth, r);
  cms_init(&(midasR->numCurrentSrc), current_width, current_depth);
  ns_init(&(midasR->ns_numTotalSrc), total_width, total_depth, r);
  cms_init(&(midasR->numCurrentDst), current_width, current_depth);
  ns_init(&(midasR->ns_numTotalDst), total_width, total_depth, r);

  midasR->current_ts = 1;

  return midasR;
}

static double ComputeScore(double a, double s, double t) {
  return s == 0 || t - 1 == 0 ? 0 : pow((a - s / t) * t, 2) / (s * (t - 1));
}

// return max score in 3 double
static double max(double a, double b, double c) {
  return (a > b) ? (a > c ? a : c) : (b > c ? b : c);
}

double midasR_Operator(MidasR *midasR, Input input) {

  if (input.ts > midasR->current_ts) {

    double factor = midasR->factor;

    multipleAll(&(midasR->numCurrentEdge), factor);
    multipleAll(&(midasR->numCurrentSrc), factor);
    multipleAll(&(midasR->numCurrentDst), factor);
    midasR->current_ts = input.ts;
  }

  char src[32], dst[32], edge[32];

  sprintf(edge, "%d", combine(input.src, input.dst));
  cms_add(&(midasR->numCurrentEdge), edge);
  cms_add(&(midasR->numTotalEdge), edge);

  sprintf(src, "%d", input.src);
  cms_add(&(midasR->numCurrentSrc), src);
  cms_add(&(midasR->numTotalSrc), src);

  sprintf(dst, "%d", input.dst);
  cms_add(&(midasR->numCurrentDst), dst);
  cms_add(&(midasR->numTotalDst), dst);

  return max(ComputeScore(cms_check(&(midasR->numCurrentEdge), edge),
                          cms_check(&(midasR->numTotalEdge), edge), input.ts),
             ComputeScore(cms_check(&(midasR->numCurrentSrc), src),
                          cms_check(&(midasR->numTotalSrc), src), input.ts),
             ComputeScore(cms_check(&(midasR->numCurrentDst), dst),
                          cms_check(&(midasR->numTotalDst), dst), input.ts));
}

double midasR_Plus_Operator(MidasR *midasR, Input input, double prob) {

  if (input.ts > midasR->current_ts) {

    double factor = midasR->factor;

    multipleAllAVX(&(midasR->numCurrentEdge), factor);
    multipleAllAVX(&(midasR->numCurrentSrc), factor);
    multipleAllAVX(&(midasR->numCurrentDst), factor);

    midasR->current_ts = input.ts;
  }

  char src[32], dst[32], edge[32];

  sprintf(edge, "%d", combine(input.src, input.dst));
  cms_add_fast(&(midasR->numCurrentEdge), edge);
  ns_add(&(midasR->ns_numTotalEdge), edge, 1.0, prob);

  sprintf(src, "%d", input.src);
  cms_add_fast(&(midasR->numCurrentSrc), src);
  ns_add(&(midasR->ns_numTotalSrc), src, 1.0, prob);

  sprintf(dst, "%d", input.dst);
  cms_add_fast(&(midasR->numCurrentDst), dst);
  ns_add(&(midasR->ns_numTotalDst), dst, 1.0, prob);

  return max(
      ComputeScore(cms_check_fast(&(midasR->numCurrentEdge)),
                   ns_check_median_fast(&(midasR->ns_numTotalEdge), edge),
                   input.ts),
      ComputeScore(cms_check_fast(&(midasR->numCurrentSrc)),
                   ns_check_median_fast(&(midasR->ns_numTotalSrc), src),
                   input.ts),
      ComputeScore(cms_check_fast(&(midasR->numCurrentDst)),
                   ns_check_median_fast(&(midasR->ns_numTotalDst), dst),
                   input.ts));

  // return 1;
}
void midasR_Free(MidasR *midasR) {
  cms_destroy(&(midasR->numCurrentEdge));
  cms_destroy(&(midasR->numTotalEdge));
  cms_destroy(&(midasR->numCurrentSrc));
  cms_destroy(&(midasR->numTotalSrc));
  cms_destroy(&(midasR->numCurrentDst));
  cms_destroy(&(midasR->numTotalDst));
  free(midasR);
}

void midasR_Plus_Free(MidasR *midasR) {
  cms_destroy(&(midasR->numCurrentEdge));
  ns_destroy(&(midasR->ns_numTotalEdge));
  cms_destroy(&(midasR->numCurrentSrc));
  ns_destroy(&(midasR->ns_numTotalSrc));
  cms_destroy(&(midasR->numCurrentDst));
  ns_destroy(&(midasR->ns_numTotalDst));
  free(midasR);
}

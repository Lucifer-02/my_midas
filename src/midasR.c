#include "midasR.h"
#include "count_min_sketch.h"

MidasR *midasRInit(int total_depth, int total_width, int current_depth,
                   int current_width, double factor) {
  MidasR *midasR = malloc(sizeof(MidasR));
  midasR->factor = factor;

  cms_init(&(midasR->numCurrentEdge), current_width, current_depth); cms_init(&(midasR->numTotalEdge), total_width, total_depth);
  cms_init(&(midasR->numCurrentSource), current_width, current_depth);
  cms_init(&(midasR->numTotalSource), total_width, total_depth);
  cms_init(&(midasR->numCurrentDestination), current_width, current_depth);
  cms_init(&(midasR->numTotalDestination), total_width, total_depth);

  midasR->current_ts = 1;

  return midasR;
}

MidasR *nitro_midasRInit(int total_depth, int total_width, int current_depth,
                         int current_width, double factor, gsl_rng *r) {

  MidasR *midasR = malloc(sizeof(MidasR));
  midasR->factor = factor;

  cms_init(&(midasR->numCurrentEdge), current_width, current_depth);
  ns_init(&(midasR->ns_numTotalEdge), total_width, total_depth, r);
  cms_init(&(midasR->numCurrentSource), current_width, current_depth);
  ns_init(&(midasR->ns_numTotalSource), total_width, total_depth, r);
  cms_init(&(midasR->numCurrentDestination), current_width, current_depth);
  ns_init(&(midasR->ns_numTotalDestination), total_width, total_depth, r);

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

double midasROperator(MidasR *midasR, Input input) {

  if (input.ts > midasR->current_ts) {

    int width = midasR->numCurrentEdge.width;
    int depth = midasR->numCurrentEdge.depth;
    double factor = midasR->factor;

    multipleAll(&(midasR->numCurrentEdge), factor, width, depth);
    multipleAll(&(midasR->numCurrentSource), factor, width, depth);
    multipleAll(&(midasR->numCurrentDestination), factor, width, depth);
    midasR->current_ts = input.ts;
  }

  char hash_src[100], hash_dst[100], hash_edge[100];

  sprintf(hash_edge, "%d", input.src * 17 + input.dst * 13);
  cms_add(&(midasR->numCurrentEdge), hash_edge);
  cms_add(&(midasR->numTotalEdge), hash_edge);

  sprintf(hash_src, "%d", input.src);
  cms_add(&(midasR->numCurrentSource), hash_src);
  cms_add(&(midasR->numTotalSource), hash_src);

  sprintf(hash_dst, "%d", input.dst);
  cms_add(&(midasR->numCurrentDestination), hash_dst);
  cms_add(&(midasR->numTotalDestination), hash_dst);

  return max(
      ComputeScore(cms_check(&(midasR->numCurrentEdge), hash_edge),
                   cms_check(&(midasR->numTotalEdge), hash_edge), input.ts),
      ComputeScore(cms_check(&(midasR->numCurrentSource), hash_src),
                   cms_check(&(midasR->numTotalSource), hash_src), input.ts),
      ComputeScore(cms_check(&(midasR->numCurrentDestination), hash_dst),
                   cms_check(&(midasR->numTotalDestination), hash_dst),
                   input.ts));

  /** return 1; */
}

double nitro_midasROperator(MidasR *midasR, Input input) {

  if (input.ts > midasR->current_ts) {

    int width = midasR->numCurrentEdge.width;
    int depth = midasR->numCurrentEdge.depth;

    double factor = midasR->factor;

    multipleAllAVX(&(midasR->numCurrentEdge), factor, width, depth);
    multipleAllAVX(&(midasR->numCurrentSource), factor, width, depth);
    multipleAllAVX(&(midasR->numCurrentDestination), factor, width, depth);

    midasR->current_ts = input.ts;
  }

  char hash_src[100], hash_dst[100], hash_edge[100];

  sprintf(hash_edge, "%d", input.src * 17 + input.dst * 13);
  cms_add_fast(&(midasR->numCurrentEdge), hash_edge);
  ns_add(&(midasR->ns_numTotalEdge), hash_edge, 1.0, 0.125);

  sprintf(hash_src, "%d", input.src);
  cms_add_fast(&(midasR->numCurrentSource), hash_src);
  ns_add(&(midasR->ns_numTotalSource), hash_src, 1.0, 0.125);

  sprintf(hash_dst, "%d", input.dst);
  cms_add_fast(&(midasR->numCurrentDestination), hash_dst);
  ns_add(&(midasR->ns_numTotalDestination), hash_dst, 1.0, 0.125);

  return max(
      ComputeScore(cms_check_fast(&(midasR->numCurrentEdge), hash_edge),
                   ns_check_mean_fast(&(midasR->ns_numTotalEdge), hash_edge),
                   input.ts),
      ComputeScore(cms_check_fast(&(midasR->numCurrentSource), hash_src),
                   ns_check_mean_fast(&(midasR->ns_numTotalSource), hash_src),
                   input.ts),
      ComputeScore(cms_check_fast(&(midasR->numCurrentDestination), hash_dst),
                   ns_check_mean_fast(&(midasR->ns_numTotalDestination), hash_dst),
                   input.ts));

  return 1;
}
void midasRFree(MidasR *midasR) {
  cms_destroy(&(midasR->numCurrentEdge));
  cms_destroy(&(midasR->numTotalEdge));
  cms_destroy(&(midasR->numCurrentSource));
  cms_destroy(&(midasR->numTotalSource));
  cms_destroy(&(midasR->numCurrentDestination));
  cms_destroy(&(midasR->numTotalDestination));
  free(midasR);
}

void midasR_Nitro_Free(MidasR *midasR) {
  cms_destroy(&(midasR->numCurrentEdge));
  ns_destroy(&(midasR->ns_numTotalEdge));
  cms_destroy(&(midasR->numCurrentSource));
  ns_destroy(&(midasR->ns_numTotalSource));
  cms_destroy(&(midasR->numCurrentDestination));
  ns_destroy(&(midasR->ns_numTotalDestination));
  free(midasR);
}

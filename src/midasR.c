#include "midasR.h"
#include "count_min_sketch.h"

MidasR *midasRInit(int total_depth, int total_width, int current_depth,
                   int current_width, double factor) {
  MidasR *midasR = malloc(sizeof(MidasR));
  midasR->factor = factor;
  cms_init(&(midasR->numCurrentEdge), current_width, current_depth);
  cms_init(&(midasR->numTotalEdge), total_width, total_depth);
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
  double max = a;
  if (b > max) {
    max = b;
  }
  if (c > max) {
    max = c;
  }
  return max;
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

double geo_midasROperator(MidasR *midasR, Input input, gsl_rng *r) {
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
  geo_add(&(midasR->numTotalEdge), hash_edge, 1.0, 0.25, r);

  sprintf(hash_src, "%d", input.src);
  cms_add(&(midasR->numCurrentSource), hash_src);
  geo_add(&(midasR->numTotalSource), hash_src, 1.0, 0.25, r);

  sprintf(hash_dst, "%d", input.dst);
  cms_add(&(midasR->numCurrentDestination), hash_dst);
  geo_add(&(midasR->numTotalDestination), hash_dst, 1.0, 0.25, r);

  return max(
      ComputeScore(cms_check(&(midasR->numCurrentEdge), hash_edge),
                   cms_check_median(&(midasR->numTotalEdge), hash_edge),
                   input.ts),
      ComputeScore(cms_check(&(midasR->numCurrentSource), hash_src),
                   cms_check_median(&(midasR->numTotalSource), hash_src),
                   input.ts),
      ComputeScore(cms_check(&(midasR->numCurrentDestination), hash_dst),
                   cms_check_median(&(midasR->numTotalDestination), hash_dst),
                   input.ts));
}
double new_midasROperator(MidasR *midasR, Input input) {
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
  my_add(&(midasR->numTotalEdge), hash_edge, 1.0, 0.25);

  sprintf(hash_src, "%d", input.src);
  cms_add(&(midasR->numCurrentSource), hash_src);
  my_add(&(midasR->numTotalSource), hash_src, 1.0, 0.25);

  sprintf(hash_dst, "%d", input.dst);
  cms_add(&(midasR->numCurrentDestination), hash_dst);
  my_add(&(midasR->numTotalDestination), hash_dst, 1.0, 0.25);

  /** static int i = 1000; */
  /** if (i-- > 0) { */
  /**   printf("Current: %lf, Total: %lf\n", */
  /**          cms_check(&(midasR->numCurrentEdge), hash_edge), */
  /**          cms_check(&(midasR->numTotalEdge), hash_edge)); */
  /** } */

  return max(
      ComputeScore(cms_check(&(midasR->numCurrentEdge), hash_edge),
                   cms_check_median(&(midasR->numTotalEdge), hash_edge),
                   input.ts),
      ComputeScore(cms_check(&(midasR->numCurrentSource), hash_src),
                   cms_check_median(&(midasR->numTotalSource), hash_src),
                   input.ts),
      ComputeScore(cms_check(&(midasR->numCurrentDestination), hash_dst),
                   cms_check_median(&(midasR->numTotalDestination), hash_dst),
                   input.ts));
}

double nitro_midasROperator(MidasR *midasR, Input input) {

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
  ns_add(&(midasR->ns_numTotalEdge), hash_edge, 1.0, 0.0125);

  sprintf(hash_src, "%d", input.src);
  cms_add(&(midasR->numCurrentSource), hash_src);
  ns_add(&(midasR->ns_numTotalSource), hash_src, 1.0, 0.0125);

  sprintf(hash_dst, "%d", input.dst);
  cms_add(&(midasR->numCurrentDestination), hash_dst);
  ns_add(&(midasR->ns_numTotalDestination), hash_dst, 1.0, 0.0125);

  return max(
      ComputeScore(cms_check(&(midasR->numCurrentEdge), hash_edge),
                   ns_check_mean(&(midasR->ns_numTotalEdge), hash_edge),
                   input.ts),
      ComputeScore(cms_check(&(midasR->numCurrentSource), hash_src),
                   ns_check_mean(&(midasR->ns_numTotalSource), hash_src),
                   input.ts),
      ComputeScore(cms_check(&(midasR->numCurrentDestination), hash_dst),
                   ns_check_mean(&(midasR->ns_numTotalDestination), hash_dst),
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

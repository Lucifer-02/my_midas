#include "midasR.h"
#include "count_min_sketch.h"

MidasR *midasRInit(int depth, int width, double factor) {
  MidasR *midasR = malloc(sizeof(MidasR));
  midasR->factor = factor;
  cms_init(&(midasR->numCurrentEdge), width, depth);
  cms_init(&(midasR->numTotalEdge), width, depth);
  cms_init(&(midasR->numCurrentSource), width, depth);
  cms_init(&(midasR->numTotalSource), width, depth);
  cms_init(&(midasR->numCurrentDestination), width, depth);
  cms_init(&(midasR->numTotalDestination), width, depth);
  midasR->current_ts = 1;

  // print error rate and confidence
  printf("MIDAS-R: Depth: %d, Width: %d, Error rate: %f, Confidence: %f\n",
         depth, width, midasR->numCurrentEdge.error_rate,
         midasR->numCurrentEdge.confidence);

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

  /** static int i = 1000; */
  /** if (i-- > 0) { */
  /**   printf("Current: %lf, Total: %lf\n", */
  /**          cms_check(&(midasR->numCurrentEdge), hash_edge), */
  /**          cms_check(&(midasR->numTotalEdge), hash_edge)); */
  /** } */

  return max(
      ComputeScore(cms_check(&(midasR->numCurrentEdge), hash_edge),
                   cms_check(&(midasR->numTotalEdge), hash_edge), input.ts),
      ComputeScore(cms_check(&(midasR->numCurrentSource), hash_src),
                   cms_check(&(midasR->numTotalSource), hash_src), input.ts),
      ComputeScore(cms_check(&(midasR->numCurrentDestination), hash_dst),
                   cms_check(&(midasR->numTotalDestination), hash_dst),
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
  my_add(&(midasR->numTotalEdge), hash_edge, 1.0);

  sprintf(hash_src, "%d", input.src);
  cms_add(&(midasR->numCurrentSource), hash_src);
  my_add(&(midasR->numTotalSource), hash_src, 1.0);

  sprintf(hash_dst, "%d", input.dst);
  cms_add(&(midasR->numCurrentDestination), hash_dst);
  my_add(&(midasR->numTotalDestination), hash_dst, 1.0);

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
void midasRFree(MidasR *midasR) {
  cms_destroy(&(midasR->numCurrentEdge));
  cms_destroy(&(midasR->numTotalEdge));
  cms_destroy(&(midasR->numCurrentSource));
  cms_destroy(&(midasR->numTotalSource));
  cms_destroy(&(midasR->numCurrentDestination));
  cms_destroy(&(midasR->numTotalDestination));
  free(midasR);
}

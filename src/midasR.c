#include "midasR.h"

MidasR *midasRInit(int depth, int width, double factor) {
  MidasR *midasR = malloc(sizeof(MidasR));
  midasR->factor = factor;
  cms_init(&(midasR->numCurrentEdge), width, depth);
  cms_init(&(midasR->numTotalEdge), width, depth);
  cms_init(&(midasR->numCurrentSource), width, depth);
  cms_init(&(midasR->numTotalSource), width, depth);
  cms_init(&(midasR->numCurrentDestination), width, depth);
  cms_init(&(midasR->numTotalDestination), width, depth);
  midasR->current_ts = 0;

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

    multipleAll(&(midasR->numCurrentEdge), midasR->factor,
                midasR->numCurrentEdge.width, midasR->numCurrentEdge.depth);
    multipleAll(&(midasR->numCurrentSource), midasR->factor,
                midasR->numCurrentSource.width, midasR->numCurrentSource.depth);
    multipleAll(&(midasR->numCurrentDestination), midasR->factor,
                midasR->numCurrentDestination.width,
                midasR->numCurrentDestination.depth);
    midasR->current_ts = input.ts;
  }

  char hash_src[100], hash_dst[100], hash_edge[100];

  sprintf(hash_edge, "%d", input.src + input.dst);
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
}

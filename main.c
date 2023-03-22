#include "gsl/gsl_rng.h"
#include "src/auroc.h"
#include "src/count_min_sketch.h"
#include "src/midas.h"
/** #include "src/midasR.h" */
#include "src/prepare.h"

#include <time.h>

int main(int argc, char const *argv[]) {

  if (argc != 4) {
    printf("Usage: ./main <data_file> <shape_file> <truth_file>\n");
    return 1;
  }

  // get number of packets
  long const N = get_shape(argv[2]);

  // get source, destination, and timestamp
  int *const src = malloc(N * sizeof(int));
  int *const dst = malloc(N * sizeof(int));
  int *const ts = malloc(N * sizeof(int));

  double *const labels = malloc(N * sizeof(double));
  double *const scores = malloc(N * sizeof(double));
  read_data(argv[1], src, dst, ts, N); // read truth labels
  read_labels(argv[3], labels, N);

  int widths[] = {65536};
  int depths[] = {8};
  int num_widths = sizeof(widths) / sizeof(widths[0]);
  int num_depths = sizeof(depths) / sizeof(depths[0]);

  // setup time to execute measurement
  clock_t start_time, end_time;
  double total_time;

  for (int k = 0; k < num_depths; k++) {
    int depth = depths[k];
    for (int i = 0; i < num_widths; i++) {

      int width = widths[i];
      /** //--------------------------------------------- */
      /** // Original MIDAS-R */
      /** start_time = clock(); */
      /**  */
      /** MidasR *midasR = midasRInit(depth, width, 0.5); */
      /**  */
      /** for (int j = 0; j < N; j++) { */
      /**  */
      /**   Input const input = {.src = src[j], .dst = dst[j], .ts = ts[j]}; */
      /**   scores[j] = midasROperator(midasR, input); */
      /** } */
      /**  */
      /** end_time = clock(); */
      /**  */
      /** // get total time */
      /** total_time = (double)(end_time - start_time) / CLOCKS_PER_SEC; */
      /** printf("Total time: %lf\n", total_time); */
      /**  */
      /** [> // compute AUROC <] */
      /** [> double const auroc1 = AUROC(labels, scores, N); <] */
      /** [> printf("AUROC: %lf\n", auroc1); <] */
      /**  */
      /** midasRFree(midasR); */
      /**  */
      /** //--------------------------------------------- */
      /** // new MIDAS-R */
      /**  */
      /** start_time = clock(); */
      /**  */
      /** MidasR *midasR_new = midasRInit(depth, width, 0.5); */
      /**  */
      /** for (int j = 0; j < N; j++) { */
      /**  */
      /**   Input const input = {.src = src[j], .dst = dst[j], .ts = ts[j]}; */
      /**   scores[j] = new_midasROperator(midasR_new, input); */
      /** } */
      /**  */
      /** end_time = clock(); */
      /**  */
      /** // get total time */
      /** total_time = (double)(end_time - start_time) / CLOCKS_PER_SEC; */
      /** printf("Total time: %lf\n", total_time); */
      /**  */
      /** [> // compute AUROC <] */
      /** [> double const auroc2 = AUROC(labels, scores, N); <] */
      /** [> printf("AUROC: %lf\n", auroc2); <] */
      /**  */
      /** midasRFree(midasR_new); */
      /**  */
      /** //--------------------------------------------- */
      /** // geo MIDAS-R */
      /**  */
      /** start_time = clock(); */
      /**  */
      /** MidasR *midasR_geo = midasRInit(depth, widths, 0.5); */
      /**  */
      /** for (int j = 0; j < N; j++) { */
      /**  */
      /**   Input const input = {.src = src[j], .dst = dst[j], .ts = ts[j]}; */
      /**   scores[j] = geo_midasROperator(midasR_geo, input); */
      /** } */
      /**  */
      /** end_time = clock(); */
      /**  */
      /** // get total time */
      /** total_time = (double)(end_time - start_time) / CLOCKS_PER_SEC; */
      /** printf("Total time: %lf\n", total_time); */
      /**  */
      /** [> // compute AUROC <] */
      /** [> double const auroc3 = AUROC(labels, scores, N); <] */
      /** [> printf("AUROC: %lf\n", auroc3); <] */
      /**  */
      /** midasRFree(midasR_geo); */

      //---------------------------------------------

      /** Midas *midas = midasInit(depth, width, 2, 1024); */
      /** // print error rate and confidence */
      /** printf("MIDAS : Depth: %d, Width: %d\n", depth, width); */
      /**  */
      /** start_time = clock(); */
      /**  */
      /** for (int j = 0; j < N; j++) { */
      /**  */
      /**   Input const input = {.src = src[j], .dst = dst[j], .ts = ts[j]}; */
      /**   scores[j] = midasOperator(midas, input); */
      /** } */
      /**  */
      /** end_time = clock(); */
      /**  */
      /** // get total time <] */
      /** total_time = (double)(end_time - start_time) / CLOCKS_PER_SEC; */
      /** printf("Total time: %lf\n", total_time); */
      /**  */
      /** midasFree(midas); */

      //---------------------------------------------

      /** gsl_rng *r = gsl_rng_alloc(gsl_rng_default); // Allocate a random
       * number */
      /** gsl_rng_set(r, time(NULL));                  // Seed the generator */
      /**  */
      /** Midas *midas_geo = midasInit(depth, width, 2, 1024); */
      /**  */
      /** // print error rate and confidence */
      /** printf("MIDAS : Depth: %d, Width: %d\n", depth, width); */
      /**  */
      /** start_time = clock(); */
      /**  */
      /** for (int j = 0; j < N; j++) { */
      /**  */
      /**   Input const input = {.src = src[j], .dst = dst[j], .ts = ts[j]}; */
      /**   scores[j] = geo_midasOperator(midas_geo, input, r); */
      /** } */
      /**  */
      /** end_time = clock(); */
      /**  */
      /** // get total time */
      /** total_time = (double)(end_time - start_time) / CLOCKS_PER_SEC; */
      /** printf("Total time: %lf\n", total_time); */
      /**  */
      /** gsl_rng_free(r); // Free the generator */
      /** midasFree(midas_geo); */

      //---------------------------------------------

      /** Midas *midas1 = midasInit(depth, width, 2, 1024); */
      /** // print error rate and confidence */
      /** printf("MIDAS : Depth: %d, Width: %d\n", depth, width); */
      /**  */
      /** start_time = clock(); */
      /**  */
      /** for (int j = 0; j < N; j++) { */
      /**  */
      /**   Input const input = {.src = src[j], .dst = dst[j], .ts = ts[j]}; */
      /**   scores[j] = midasOperator(midas1, input); */
      /** } */
      /**  */
      /** end_time = clock(); */
      /**  */
      /** // get total time <] */
      /** total_time = (double)(end_time - start_time) / CLOCKS_PER_SEC; */
      /** printf("Total time: %lf\n", total_time); */
      /**  */
      /** midasFree(midas1); */

      //---------------------------------------------

      /** Midas *midas1 = midasInit(depth, width, 2, 1024); */
      /** for (int j = 0; j < N; j++) { */
      /**  */
      /**   Input const input = {.src = src[j], .dst = dst[j], .ts = ts[j]}; */
      /**   scores[j] = midasOperator(midas1, input); */
      /** } */

      //---------------------------------------------

      gsl_rng *r = gsl_rng_alloc(gsl_rng_default); // Allocate a random number
      gsl_rng_set(r, time(NULL));                  // Seed the generator

      Midas *midas_geo = midasInit(depth, width, 2, 1024);

      uint32_t row = 0;
      for (int j = 0; j < N; j++) {

        Input const input = {.src = src[j], .dst = dst[j], .ts = ts[j]};
        scores[j] = geo_midasOperator(midas_geo, input, r, &row);
      }
    }
  };

  /** FILE *const fscore = fopen("./temp/Score.txt", "w"); */
  /** for (int i = 0; i < N; i++) { */
  /**  */
  /**   // write to file */
  /**   fprintf(fscore, "%lf\n", scores[i]); */
  /** }; */
  /** fclose(fscore); */

  free(labels);
  free(scores);
  free(src);
  free(dst);
  free(ts);

  return 0;
}

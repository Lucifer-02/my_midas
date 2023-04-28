#include "gsl/gsl_rng.h"
#include "src/auroc.h"
#include "src/midas.h"
#include "src/midasR.h"
#include "src/prepare.h"

#include <stdio.h>
#include <time.h>

int main(int argc, char const *argv[]) {

  if (argc != 5) {
    printf(
        "Usage: ./main <data_file> <shape_file> <truth_file> <density_file>\n");
    return 1;
  }

  // get number of packets
  long const N = get_shape(argv[2]);

  // get source, destination, and timestamp
  int *const src = malloc(N * sizeof(int));
  int *const dst = malloc(N * sizeof(int));
  int *const ts = malloc(N * sizeof(int));

  double *const labels = malloc(N * sizeof(double));
  int *const density = malloc(N * sizeof(int));
  double *const scores = malloc(N * sizeof(double));
  read_data(argv[1], src, dst, ts, N); // read truth labels
  read_labels(argv[3], labels, N);
  read_density(argv[4], density, N);

  int widths[] = {8192};
  int depths[] = {8};
  double factors[] = {0.9};
  int num_widths = sizeof(widths) / sizeof(widths[0]);
  int num_depths = sizeof(depths) / sizeof(depths[0]);
  int num_factors = sizeof(factors) / sizeof(factors[0]);


  // setup time to execute measurement
  clock_t start_time, end_time;
  double total_time;

  const int REPEAT = 1;

  for (int j = 0; j < REPEAT; j++) {
    for (int k = 0; k < num_depths; k++) {
      int depth = depths[k];
      for (int i = 0; i < num_widths; i++) {
        for (int l = 0; l < num_factors; l++) {

          printf("=============================================\n");
          double factor = factors[l];
          printf("Factor: %lf\n", factor);
          int width = widths[i];

#if FULL
//---------------------------------------------
#if MIDAS_R
          // Original MIDAS-R

          MidasR *midasR = midasR_Init(depth, width, 4, 1024, factor);

          printf("MIDAS-R: Depth: %d, Width: %d\n", depth, width);

          start_time = clock();

          for (int j = 0; j < N; j++) {

            Input const input = {.src = src[j], .dst = dst[j], .ts = ts[j]};
            scores[j] = midasR_Operator(midasR, input);
          }

          end_time = clock();

          // get total time
          total_time = (double)(end_time - start_time) / CLOCKS_PER_SEC;
          printf("\t|Total time: %lf\n", total_time);
          // printf("%lf\n", total_time);

#if AUC
          // compute AUROC
          double const auroc1 = AUROC(labels, scores, N);
          printf("\t|AUROC: %lf\n", auroc1);
#endif

          midasR_Free(midasR);

#endif

//---------------------------------------------
// MIDAS-R+
#if MIDAS_R_PLUS
          gsl_rng *nr1 = gsl_rng_alloc(gsl_rng_default); // allocate a random
          gsl_rng_set(nr1, time(NULL));                  // seed the generator

          MidasR *midasR_plus=
              midasR_Plus_Init(depth, width, 4, 1024, factor, nr1);
          double prob4;

          printf("MIDAS-R+: Depth: %d, Width: %d\n", depth, width);

          start_time = clock();

          for (int j = 0; j < N; j++) {

            Input const input = {.src = src[j], .dst = dst[j], .ts = ts[j]};

            prob4 = 1.0 * (density[j] < 5) +
                    0.25 * (density[j] >= 5 && density[j] < 10) +
                    0.125 * (density[j] >= 10 && density[j] < 20) +
                    0.0625 * (density[j] >= 20);
            scores[j] = midasR_Plus_Operator(midasR_plus, input, prob4);
          }

          end_time = clock();

          // get total time
          total_time = (double)(end_time - start_time) / CLOCKS_PER_SEC;
          printf("\t|Total time: %lf\n", total_time);
          // printf("%lf\n", total_time);

#if AUC
          // compute AUROC
          double const auroc2 = AUROC(labels, scores, N);
          printf("\t|AUROC: %lf\n", auroc2);
#endif

          midasR_Plus_Free(midasR_plus);
          gsl_rng_free(nr1);

#endif

          //---------------------------------------------
          // Original MIDAS
#if MIDAS

          Midas *midas = midas_Init(depth, width, depth, width);
          // print error rate and confidence
          printf("MIDAS : Depth: %d, Width: %d\n", depth, width);

          start_time = clock();

          for (int j = 0; j < N; j++) {

            Input const input = {.src = src[j], .dst = dst[j], .ts = ts[j]};
            scores[j] = midas_Operator(midas, input);
          }

          end_time = clock();

          // get total time <]
          total_time = (double)(end_time - start_time) / CLOCKS_PER_SEC;
          printf("\t|Total time: %lf\n", total_time);

#if AUC
          // compute AUROC
          double const auroc = AUROC(labels, scores, N);
          printf("\t|AUROC: %lf\n", auroc);
#endif

          midas_Free(midas);

#endif
//---------------------------------------------
// MIDAS+
#if MIDAS_PLUS

          gsl_rng *nr = gsl_rng_alloc(gsl_rng_default); // allocate a random
          gsl_rng_set(nr, time(NULL));                  // seed the generator

          Midas *midas_plus= midas_Plus_Init(depth, width, depth, width, nr);
          double prob8 = 1;

          // print error rate and confidence
          printf("MIDAS+: depth: %d, width: %d\n", depth, width);

          start_time = clock();

          for (int j = 0; j < N; j++) {

            Input const input = {.src = src[j], .dst = dst[j], .ts = ts[j]};

            // rewrite above code using branchless programming
            prob8 = 1.0 * (density[j] < 5) +
                    0.25 * (density[j] >= 5 && density[j] < 10) +
                    0.125 * (density[j] >= 10 && density[j] < 20) +
                    0.0625 * (density[j] >= 20);

            scores[j] = midas_Plus_Operator(midas_plus, input, prob8);
          }

          end_time = clock();

          // get total time
          total_time = (double)(end_time - start_time) / CLOCKS_PER_SEC;
          printf("\t|total time: %lf\n", total_time);

#if AUC
          // compute auroc
          double const auroc4 = AUROC(labels, scores, N);
          printf("\t|AUROC: %lf\n", auroc4);
#endif
          gsl_rng_free(nr); // free the generator
          midas_Plus_Free(midas_plus);

#endif

          //---------------------------------------------
          //
#else
#if MIDAS_PLUS
          // MIDAS+
          gsl_rng *nr = gsl_rng_alloc(gsl_rng_default); // allocate a random
          gsl_rng_set(nr, time(NULL));                  // seed the generator

          Midas *midas_plus= midas_Plus_Init(depth, width, depth, width, nr);
          double prob = 1;

          for (int j = 0; j < N; j++) {

            Input const input = {.src = src[j], .dst = dst[j], .ts = ts[j]};

            // if (density[j] < 5)
            //   prob = 0.5;
            // else if (5 <= density[j] && density[j] < 20)
            //   prob = 0.25;
            // else if (density[j] > 20)
            //   prob = 0.125;

            // rewrite above code using branchless programming
            prob = 0.5 * (density[j] < 5) +
                   0.25 * (density[j] >= 5 && density[j] < 20) +
                   0.125 * (density[j] >= 20);

            scores[j] = midas_Plus_Operator(midas_plus, input, prob);
          }
#endif

//---------------------------------------------
#if MIDAS
          // Original MIDAS

          Midas *midas = midas_Init(depth, width, depth, width);

          for (int j = 0; j < N; j++) {

            Input const input = {.src = src[j], .dst = dst[j], .ts = ts[j]};
            scores[j] = midas_Operator(midas, input);
          }
#endif

//---------------------------------------------
// Original MIDAS-R
#if MIDAS_R

          MidasR *midasR = midasR_Init(depth, width, depth, width, 0.5);

          for (int j = 0; j < N; j++) {

            Input const input = {.src = src[j], .dst = dst[j], .ts = ts[j]};
            scores[j] = midasR_Operator(midasR, input);
          }

#endif

//---------------------------------------------
// MIDAS-R+
#if MIDAS_R_PLUS

          gsl_rng *nr1 = gsl_rng_alloc(gsl_rng_default); // allocate a random
          gsl_rng_set(nr1, time(NULL));                  // seed the generator
          double prob1 = 1;

          MidasR *midasR_plus=
              midasR_Plus_Init(depth, width, depth, width, 0.5, nr1);

          for (int j = 0; j < N; j++) {

            Input const input = {.src = src[j], .dst = dst[j], .ts = ts[j]};
            prob1 = 1.0 * (density[j] < 5) +
                    0.25 * (density[j] >= 5 && density[j] < 20) +
                    0.125 * (density[j] >= 20);
            scores[j] = midasR_Plus_Operator(midasR_plus, input, prob1);
          }
#endif
#endif
        }
      }
    }
  }

  FILE *fscore = fopen("output/Score.txt", "w");
  if (fscore == NULL) {
    printf("Error opening file!\n");
    exit(1);
  }
  for (int i = 0; i < N; i++) {

    // write to file
    fprintf(fscore, "%lf\n", scores[i]);

  };
  fclose(fscore);

  free(density);
  free(labels);
  free(scores);
  free(src);
  free(dst);
  free(ts);

  return 0;
}

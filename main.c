#include "src/auroc.h"
#include "src/count_min_sketch.h"
#include "src/midas.h"
#include "src/midasR.h"
#include "src/prepare.h"

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

  read_data(argv[1], src, dst, ts, N);
  // read truth labels
  read_labels(argv[3], labels, N);

  int widths[] = {1024};
  int depth = 2;
  int num_width = sizeof(widths) / sizeof(widths[0]);

  for (int i = 0; i < num_width; i++) {

    MidasR *midasR = midasRInit(depth, widths[i], 0.5);

    for (int j = 0; j < N; j++) {

      Input const input = {.src = src[j], .dst = dst[j], .ts = ts[j]};
      scores[j] = midasROperator(midasR, input);
    }

    // compute AUROC
    double const aurocR = AUROC(labels, scores, N);
    printf("AUROC: %lf\n", aurocR);

    //---------------------------------------------
    /** Midas *midas = midasInit(depth, widths[i]); */
    /**  */
    /** for (int j = 0; j < N; j++) { */
    /**  */
    /**   Input const input = {.src = src[j], .dst = dst[j], .ts = ts[j]}; */
    /**   scores[j] = midasOperator(midas, input); */
    /** } */
    /**  */
    /** // compute AUROC */
    /** double const auroc = AUROC(labels, scores, N); */
    /** printf("AUROC: %lf\n", auroc); */
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

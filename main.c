#include "src/auroc.h"
#include "src/config.h"
#include "src/count_min_sketch.h"
#include "src/normalCore.h"

void read_labels(char const *filename, double *labels, int len) {

  FILE *f = fopen(filename, "r");
  for (int i = 0; i < len; i++) {
    fscanf(f, "%lf\n", labels + i);
  }
  fclose(f);
}

int get_shape(char const *filename) {
  FILE *f = fopen(filename, "r");
  int shape;
  fscanf(f, "%d", &shape);
  fclose(f);
  return shape;
}

// read csv file and store in 3 arrays int
void read_data(char const *filename, int *arr1, int *arr2, int *arr3, int len) {

  FILE *f_data = fopen(filename, "r");

  for (int i = 0; i < len; i++) {
    fscanf(f_data, "%d,%d,%d\n", &arr1[i], &arr2[i], &arr3[i]);
  }
  fclose(f_data);
}

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

  // Init
  Config const config = {.depth = 1, .width = 1024};
  NormalCore *const midas = midasInit(config);

  // get input and compute score
  for (int i = 0; i < N; i++) {

    Input const input = {.src = src[i], .dst = dst[i], .ts = ts[i]};
    scores[i] = normalOperator(midas, input);
  };

  // compute AUROC
  double const auroc = AUROC(labels, scores, N);
  printf("AUROC: %lf\n", auroc);

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

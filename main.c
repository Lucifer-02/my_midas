#include "src/auroc.h"
#include "src/count_min_sketch.h"
#include "src/midas.h"
#include "src/midasR.h"
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

  int widths[] = {1024 * 4};
  int depth = 8;
  int num_width = sizeof(widths) / sizeof(widths[0]);

  // setup time to execute measurement
  clock_t start_time, end_time;
  double total_time;

  for (int i = 0; i < num_width; i++) {

    /** //--------------------------------------------- */
    /** // Original MIDAS-R */
    /** start_time = clock(); */
    /**  */
    /** MidasR *midasR = midasRInit(depth, widths[i], 0.5); */
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
    /** MidasR *midasR_new = midasRInit(depth, widths[i], 0.5); */
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
    /** MidasR *midasR_geo = midasRInit(depth, widths[i], 0.5); */
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

	start_time = clock();

	Midas *midas = midasInit(depth, widths[i]);

	for (int j = 0; j < N; j++) {

	  Input const input = {.src = src[j], .dst = dst[j], .ts = ts[j]};
	  scores[j] = geo_midasOperator(midas, input);
	}

	end_time = clock();

	// get total time
	total_time = (double)(end_time - start_time) / CLOCKS_PER_SEC;
	printf("Total time: %lf\n", total_time);

	/** // compute AUROC */
	/** double const auroc = AUROC(labels, scores, N); */
	/** printf("AUROC: %lf\n", auroc); */

	midasFree(midas);
  };

  FILE *const fscore = fopen("./temp/Score.txt", "w");
  for (int i = 0; i < N; i++) {

    // write to file
    fprintf(fscore, "%lf\n", scores[i]);
  };
  fclose(fscore);

  free(labels);
  free(scores);
  free(src);
  free(dst);
  free(ts);

  return 0;
}

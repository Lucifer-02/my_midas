#include "prepare.h"
#include <stdlib.h>

void read_density(char const *const filename, int *const density,
                  long const N) {
  FILE *fp = fopen(filename, "r");
  if (fp == NULL) {
    printf("Error opening file\n");
    exit(1);
  }

  for (int i = 0; i < N; i++) {
    fscanf(fp, "%d", &density[i]);
  }

  fclose(fp);
}
void read_labels(char const *filename, double *const labels, int len) {

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
void read_data(char const *filename, int *const arr1, int *const arr2,
               int *const arr3, int const len) {

  FILE *f_data = fopen(filename, "r");

  for (int i = 0; i < len; i++) {
    fscanf(f_data, "%d,%d,%d\n", &arr1[i], &arr2[i], &arr3[i]);
  }
  fclose(f_data);
}

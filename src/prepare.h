#ifndef PREPARE_H
#define PREPARE_H

#include <stdio.h>

typedef struct {
  int src, dst, ts;
} Input;

void read_density(char const *const filename, int *const density, long const N);
void read_labels(char const *filename, double *const labels, int len);
int get_shape(char const *filename);
// read csv file and store in 3 arrays int
void read_data(char const *filename, int *const arr1, int *const arr2,
               int *const arr3, int const len);

#endif

#include "auroc.h"

// implement qsort using elements in yPred to compare
static void my_qsort(size_t *base, size_t size, const double *yPred) {
  if (size <= 1)
    return;

  size_t i = 0, j = size - 1;
  size_t pivot = i;
  while (i < j) {
    while (i < j && yPred[*(base + j)] <= yPred[*(base + pivot)])
      j--;
    while (i < j && yPred[*(base + i)] >= yPred[*(base + pivot)])
      i++;
    if (i < j) {
      size_t tmp = *(base + i);
      *(base + i) = *(base + j);
      *(base + j) = tmp;
    }
  }
  size_t tmp = *(base + i);
  *(base + i) = *(base + pivot);
  *(base + pivot) = tmp;

  my_qsort(base, i, yPred);
  my_qsort(base + (i + 1), size - i - 1, yPred);
}

/// @tparam T Type of array elements, should be a floating number type
/// @param yTrue Array of ground truth labels, 0.0 is negative, 1.0 is positive
/// @param yPred Array of predicted scores, can be of any range
/// @param n Number of elements in the array
/// @return AUROC/ROC-AUC score, range [0.0, 1.0]
double AUROC(const double *yTrue, const double *yPred, size_t n) {
  for (size_t i = 0; i < n; i++) {
    if (isnan(yPred[i]) || isinf(yPred[i]) || (yTrue[i] != 0 && yTrue[i] != 1))
      return -1;
  }

  size_t *index = malloc(n * sizeof(size_t));
  for (size_t i = 0; i < n; i++)
    index[i] = i;

  my_qsort(index, n, yPred);

  double *y = malloc(n * sizeof(double)); // Desc
  double *z = malloc(n * sizeof(double)); // Desc
  for (size_t i = 0; i < n; i++) {
    y[i] = yTrue[index[i]];
    z[i] = yPred[index[i]];
  }

  double *tp = y; // Reuse
  tp[0] = y[0];
  for (size_t i = 1; i < n; i++) {
    tp[i] = tp[i - 1] + y[i];
  }

  size_t top = 0; // # diff
  for (size_t i = 0; i < n - 1; i++) {
    if (z[i] != z[i + 1])
      index[top++] = i;
  }
  index[top++] = n - 1;
  n = top; // Size of y/z -> sizeof tps/fps
  free(z);

  double *fp = malloc(n * sizeof(double));
  for (size_t i = 0; i < n; i++) {
    tp[i] = tp[index[i]];
    fp[i] = 1 + index[i] - tp[i];
  }
  free(index);

  double *tp_diff = malloc(n * sizeof(double));
  double *fp_diff = malloc(n * sizeof(double));
  for (size_t i = 1; i < n; i++) {
    tp_diff[i] = tp[i] - tp[i - 1];

    fp_diff[i] = fp[i] - fp[i - 1];
  }

  top = 1;
  for (size_t i = 1; i < n - 1; i++) {
    if (tp_diff[i] != tp_diff[i + 1] || fp_diff[i] != fp_diff[i + 1]) {
      tp[top] = tp[i];
      fp[top] = fp[i];
      top++;
    }
  }
  tp[top] = tp[n - 1];
  fp[top] = fp[n - 1];
  n = ++top; // Size of tp/fp -> size of optimized tp/fp
  free(tp_diff);
  free(fp_diff);

  for (size_t i = 0; i < n; i++) {
    tp[i] /= tp[n - 1];
    fp[i] /= fp[n - 1];
  }

  double area = tp[0] * fp[0] / 2; // The first triangle from origin
  for (size_t i = 0; i < n - 1; i++) {
    area += (tp[i] + tp[i + 1]) * (fp[i + 1] - fp[i]) / 2;
  }

  free(tp);
  free(fp);

  return area;
}

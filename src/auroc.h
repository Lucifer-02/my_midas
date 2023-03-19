#ifndef AUROC_H
#define AUROC_H

#include <math.h>
#include <stddef.h>
#include <stdlib.h>

/// @tparam T Type of array elements, should be a floating number type
/// @param yTrue Array of ground truth labels, 0.0 is negative, 1.0 is positive
/// @param yPred Array of predicted scores, can be of any range
/// @param n Number of elements in the array
/// @return AUROC/ROC-AUC score, range [0.0, 1.0]
double AUROC(const double *yTrue, const double *yPred, size_t n);
// another version of AUROC
double calculateAUC(double estimates[], double truths[], int length);

#endif // AUROC_H

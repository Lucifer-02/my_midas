#include "nitro_sketch.h"
#include <gsl/gsl_randist.h>
#include <inttypes.h> /* PRIu64 */
#include <limits.h>
#include <math.h>
#include <pthread.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

// include AVX lib
#include <immintrin.h>

#define LOG_TWO 0.6931471805599453

/* private functions */
static int __setup_ns(NitroSketch *ns, uint32_t width, uint32_t depth,
                      double error_rate, double confidence,
                      ns_hash_function hash_function, gsl_rng *r);
static uint64_t *__default_hash(unsigned int num_hashes, const char *key);
static uint64_t __fnv_1a(const char *key, int seed);
static int __double_compare(const void *a, const void *b);

// Compatibility with non-clang compilers
#ifndef __has_builtin
#define __has_builtin(x) 0
#endif

int ns_init_optimal_alt(NitroSketch *ns, double error_rate, double confidence,
                        ns_hash_function hash_function, gsl_rng *r) {
  if (error_rate < 0 || confidence < 0) {
    fprintf(stderr, "Unable to initialize the count-min sketch since both "
                    "error_rate and confidence must be positive!\n");
    return NS_ERROR;
  }
  uint32_t width = ceil(2 / error_rate);
  uint32_t depth = ceil((-1 * log(1 - confidence)) / LOG_TWO);
  return __setup_ns(ns, width, depth, error_rate, confidence, hash_function, r);
}

int ns_init_alt(NitroSketch *ns, uint32_t width, uint32_t depth,
                ns_hash_function hash_function, gsl_rng *r) {
  if (depth < 1 || width < 1) {
    fprintf(stderr, "Unable to initialize the count-min sketch since either "
                    "width or depth is 0!\n");
    return NS_ERROR;
  }
  double confidence = 1 - (1 / pow(2, depth));
  double error_rate = 2 / (double)width;
  return __setup_ns(ns, width, depth, error_rate, confidence, hash_function, r);
}

int ns_destroy(NitroSketch *ns) {
  free(ns->bins);
  free(ns->hashes);
  free(ns->pool);
  ns->width = 0;
  ns->depth = 0;
  ns->confidence = 0.0;
  ns->error_rate = 0.0;
  ns->elements_added = 0;
  ns->hash_function = NULL;
  ns->bins = NULL;
  ns->r = NULL;
  ns->row = 0;

  return NS_SUCCESS;
}

int ns_clear(NitroSketch *ns) {

  memset(ns->bins, 0, ns->width * ns->depth * sizeof(double));
  ns->elements_added = 0;

  return NS_SUCCESS;
}

void ns_add(NitroSketch *ns, const char *key, double x, double prob) {

  for (unsigned int i = 0; i < ns->depth; ++i) {

    if (ns->row >= ns->depth) {
      ns->row -= ns->depth;
      break;
    }

    /** printf("Current row: %d\n", row); */
    uint64_t hash = __fnv_1a(key, ns->row);
    uint64_t bin = (hash % ns->width) + (ns->row * ns->width);

    ns->bins[bin] = ns->bins[bin] + x / prob;

    ns->row = (ns->row == (ns->depth - 1) ? 0 : ns->row);
    uint32_t var = gsl_ran_geometric(ns->r, prob);

    /** int var = 5; */
    ns->row += var;
  }
}

double ns_check_mean_alt(NitroSketch *ns, uint64_t *hashes,
                         unsigned int num_hashes) {
  if (num_hashes < ns->depth) {
    fprintf(stderr, "Insufficient hashes to complete the mean lookup of the "
                    "element to the count-min sketch!");
    return NS_ERROR;
  }
  double num_add = 0;
  for (unsigned int i = 0; i < ns->depth; ++i) {
    uint32_t bin = (hashes[i] % ns->width) + (i * ns->width);
    num_add += ns->bins[bin];
  }
  return num_add / ns->depth;
}

double ns_check_mean(NitroSketch *ns, const char *key) {
  uint64_t *hashes = ns_get_hashes(ns, key);
  double num_add = ns_check_mean_alt(ns, hashes, ns->depth);
  free(hashes);
  return num_add;
}

double ns_check_mean_fast(NitroSketch *ns) {
  uint64_t *hashes = ns->hashes;
  double num_add = ns_check_mean_alt(ns, hashes, ns->depth);
  return num_add;
}

static double ns_check_median_alt(NitroSketch *ns, uint64_t *hashes,
                                  unsigned int num_hashes) {
  if (num_hashes < ns->depth) {
    fprintf(stderr, "Insufficient hashes to complete the median lookup of the "
                    "element to the count-min sketch!");
    return NS_ERROR;
  }
  double num_add = 0;
  double *median_values = (double *)calloc(ns->depth, sizeof(double));
  for (unsigned int i = 0; i < ns->depth; ++i) {
    uint32_t bin = (hashes[i] % ns->width) + (i * ns->width);
    median_values[i] = ns->bins[bin];
  }
  // return the median of the median_values array... need to sort first
  qsort(median_values, ns->depth, sizeof(double), __double_compare);
  int32_t n = ns->depth;
  if (n % 2 == 0) {
    num_add = (median_values[n / 2] + median_values[n / 2 - 1]) / 2;
  } else {
    num_add = median_values[n / 2];
  }
  free(median_values);
  return num_add;
}

double median(double *a, int n) {
  int i, j, k, l, r;
  double x, t;

  l = 0;
  r = n - 1;
  k = n / 2;
  while (l < r) {
    x = a[k];
    i = l;
    j = r;
    do {
      while (a[i] < x)
        i++;
      while (a[j] > x)
        j--;
      if (i <= j) {
        t = a[i];
        a[i] = a[j];
        a[j] = t;
        i++;
        j--;
      }
    } while (i <= j);
    if (j < k)
      l = i;
    if (k < i)
      r = j;
  }
  return a[k];
}

double ns_check_median_fast(NitroSketch *ns, const char *key) {

  double num_add = 0;
  uint64_t *hashes = ns_get_hashes_fast(ns, key);
  double *median_values = ns->pool;
  for (unsigned int i = 0; i < ns->depth; ++i) {
    uint32_t bin = (hashes[i] % ns->width) + (i * ns->width);
    median_values[i] = ns->bins[bin];
  }

  num_add = median(median_values, ns->depth);
  return num_add;
}

double ns_check_median(NitroSketch *ns, const char *key) {
  uint64_t *hashes = ns_get_hashes(ns, key);
  double num_add = ns_check_median_alt(ns, hashes, ns->depth);
  free(hashes);
  return num_add;
}

uint64_t *ns_get_hashes_alt(NitroSketch *ns, unsigned int num_hashes,
                            const char *key) {
  return ns->hash_function(num_hashes, key);
}

uint64_t *ns_get_hashes_fast(NitroSketch *ns, const char *key) {

  for (unsigned int i = 0; i < ns->depth; ++i) {
    ns->hashes[i] = __fnv_1a(key, i);
  }
  return ns->hashes;
}

/*******************************************************************************
 *    PRIVATE FUNCTIONS
 *******************************************************************************/

static int __setup_ns(NitroSketch *ns, unsigned int width, unsigned int depth,
                      double error_rate, double confidence,
                      ns_hash_function hash_function, gsl_rng *r) {
  ns->width = width;
  ns->depth = depth;
  ns->confidence = confidence;
  ns->error_rate = error_rate;
  ns->elements_added = 0;
  ns->row = 0;
  ns->bins = (double *)calloc((width * depth), sizeof(double));
  ns->hash_function = (hash_function == NULL) ? __default_hash : hash_function;
  ns->r = r;
  ns->hashes = (uint64_t *)calloc(depth, sizeof(uint64_t));
  ns->pool = (double *)calloc(depth, sizeof(double));

  if (NULL == ns->bins) {
    fprintf(stderr, "Failed to allocate %zu bytes for bins!",
            ((width * depth) * sizeof(double)));
    return NS_ERROR;
  }
  return NS_SUCCESS;
}

/* NOTE: The caller will free the results */
static uint64_t *__default_hash(unsigned int num_hashes, const char *str) {
  uint64_t *results = (uint64_t *)calloc(num_hashes, sizeof(uint64_t));
  for (unsigned int i = 0; i < num_hashes; ++i) {
    results[i] = __fnv_1a(str, i);
  }

  return results;
}

static uint64_t __fnv_1a(const char *key, int seed) {
  // FNV-1a hash (http://www.isthe.com/chongo/tech/comp/fnv/)
  int i, len = strlen(key);
  uint64_t h = 14695981039346656037ULL +
               (31 * seed); // FNV_OFFSET 64 bit with magic number seed
  for (i = 0; i < len; ++i) {
    h = h ^ (unsigned char)key[i];
    h = h * 1099511628211ULL; // FNV_PRIME 64 bit
  }
  return h;
}

static int __double_compare(const void *a, const void *b) {
  return (*(double *)a - *(double *)b);
}

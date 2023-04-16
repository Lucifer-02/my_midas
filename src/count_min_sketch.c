#include "count_min_sketch.h"
#include <inttypes.h> /* PRIu64 */
#include <math.h>
#include <string.h>
#include <xxh3.h>

// include AVX lib
#include <immintrin.h>

#define LOG_TWO 0.6931471805599453

/* private functions */
static int __setup_cms(CountMinSketch *cms, uint32_t width, uint32_t depth,
                       double error_rate, double confidence,
                       cms_hash_function hash_function);
static uint64_t *__default_hash(unsigned int num_hashes, const char *key);
static uint64_t __fnv_1a(const char *key, int seed);
static int __compare(const void *a, const void *b);
static int __double_compare(const void *a, const void *b);

// Compatibility with non-clang compilers
#ifndef __has_builtin
#define __has_builtin(x) 0
#endif

static uint64_t *new_hashes(uint64_t *hashes, const char *str, int depth) {
  uint64_t hash1 = XXH64(str, strlen(str), 13);
  uint64_t delta = (hash1 >> 17) | (hash1 << 47);
  hashes[0] = hash1;
  for (int i = 1; i < depth; i++) {
    hash1 += delta;
    hashes[i] = hash1;
    delta += i;
  }
  return hashes;
}

int cms_init_optimal_alt(CountMinSketch *cms, double error_rate,
                         double confidence, cms_hash_function hash_function) {
  /* https://cs.stackexchange.com/q/44803 */
  if (error_rate < 0 || confidence < 0) {
    fprintf(stderr, "Unable to initialize the count-min sketch since both "
                    "error_rate and confidence must be positive!\n");
    return CMS_ERROR;
  }
  uint32_t width = ceil(2 / error_rate);
  uint32_t depth = ceil((-1 * log(1 - confidence)) / LOG_TWO);
  return __setup_cms(cms, width, depth, error_rate, confidence, hash_function);
}

int cms_init_alt(CountMinSketch *cms, uint32_t width, uint32_t depth,
                 cms_hash_function hash_function) {
  if (depth < 1 || width < 1) {
    fprintf(stderr, "Unable to initialize the count-min sketch since either "
                    "width or depth is 0!\n");
    return CMS_ERROR;
  }
  double confidence = 1 - (1 / pow(2, depth));
  double error_rate = 2 / (double)width;
  return __setup_cms(cms, width, depth, error_rate, confidence, hash_function);
}

int cms_destroy(CountMinSketch *cms) {
  free(cms->bins);
  if (cms->hashes != NULL)
    free(cms->hashes);
  cms->width = 0;
  cms->depth = 0;
  cms->confidence = 0.0;
  cms->error_rate = 0.0;
  cms->elements_added = 0;
  cms->hash_function = NULL;
  cms->bins = NULL;

  return CMS_SUCCESS;
}

int cms_clear(CountMinSketch *cms) {

  /** uint32_t i, j = cms->width * cms->depth; */
  /** for (i = 0; i < j; ++i) { */
  /**   cms->bins[i] = 0; */
  /** } */

  // set memory to 0
  memset(cms->bins, 0, cms->width * cms->depth * sizeof(double));
  cms->elements_added = 0;
  return CMS_SUCCESS;
}

void cms_add_inc_alt(CountMinSketch *cms, uint64_t *hashes, double x) {
  for (unsigned int i = 0; i < cms->depth; ++i) {
    uint64_t bin = (hashes[i] % cms->width) + (i * cms->width);
    cms->bins[bin] = cms->bins[bin] + x;
  }

  cms->elements_added += x;
}

void cms_add_inc(CountMinSketch *cms, const char *key, double x) {
  uint64_t *hashes = cms_get_hashes(cms, key);
  cms_add_inc_alt(cms, hashes, x);
  free(hashes);
}

void cms_add_inc_fast(CountMinSketch *cms, const char *key, double x) {
  // uint64_t *hashes = cms_get_hashes_fast(cms, key);
  uint64_t *hashes = new_hashes(cms->hashes, key, cms->depth);
  for (unsigned int i = 0; i < cms->depth; ++i) {
    uint64_t bin = (hashes[i] % cms->width) + (i * cms->width);
    cms->bins[bin] = cms->bins[bin] + x;
  }

  cms->elements_added += x;
}

double cms_check_alt(CountMinSketch *cms, uint64_t *hashes,
                     unsigned int num_hashes) {
  if (num_hashes < cms->depth) {
    fprintf(stderr, "Insufficient hashes to complete the min lookup of the "
                    "element to the count-min sketch!");
    return CMS_ERROR;
  }
  double num_add = INT32_MAX;
  for (unsigned int i = 0; i < cms->depth; ++i) {
    uint32_t bin = (hashes[i] % cms->width) + (i * cms->width);
    if (cms->bins[bin] < num_add) {
      num_add = cms->bins[bin];
    }
  }
  return num_add;
}

double cms_check(CountMinSketch *cms, const char *key) {
  uint64_t *hashes = cms_get_hashes(cms, key);
  double num_add = cms_check_alt(cms, hashes, cms->depth);
  free(hashes);
  return num_add;
}

double cms_check_fast(CountMinSketch *cms) {
  uint64_t *hashes = cms->hashes;
  double num_add = INT32_MAX;
  for (unsigned int i = 0; i < cms->depth; ++i) {
    uint32_t bin = (hashes[i] % cms->width) + (i * cms->width);
    if (cms->bins[bin] < num_add) {
      num_add = cms->bins[bin];
    }
  }
  return num_add;
}

double cms_check_mean_alt(CountMinSketch *cms, uint64_t *hashes,
                          unsigned int num_hashes) {
  if (num_hashes < cms->depth) {
    fprintf(stderr, "Insufficient hashes to complete the mean lookup of the "
                    "element to the count-min sketch!");
    return CMS_ERROR;
  }
  double num_add = 0;
  for (unsigned int i = 0; i < cms->depth; ++i) {
    uint32_t bin = (hashes[i] % cms->width) + (i * cms->width);
    num_add += cms->bins[bin];
  }
  return num_add / cms->depth;
}

double cms_check_mean(CountMinSketch *cms, const char *key) {
  uint64_t *hashes = cms_get_hashes(cms, key);
  double num_add = cms_check_mean_alt(cms, hashes, cms->depth);
  free(hashes);
  return num_add;
}

double cms_check_mean_min_alt(CountMinSketch *cms, uint64_t *hashes,
                              unsigned int num_hashes) {
  if (num_hashes < cms->depth) {
    fprintf(stderr, "Insufficient hashes to complete the mean-min lookup of "
                    "the element to the count-min sketch!");
    return CMS_ERROR;
  }
  double num_add = 0;
  double *mean_min_values = (double *)calloc(cms->depth, sizeof(double));
  for (unsigned int i = 0; i < cms->depth; ++i) {
    uint32_t bin = (hashes[i] % cms->width) + (i * cms->width);
    double val = cms->bins[bin];
    mean_min_values[i] = val - ((cms->elements_added - val) / (cms->width - 1));
  }
  // return the median of the mean_min_value array... need to sort first
  qsort(mean_min_values, cms->depth, sizeof(double), __compare);
  int32_t n = cms->depth;
  if (n % 2 == 0) {
    num_add = (mean_min_values[n / 2] + mean_min_values[n / 2 - 1]) / 2;
  } else {
    num_add = mean_min_values[n / 2];
  }
  free(mean_min_values);
  return num_add;
}

double cms_check_mean_min(CountMinSketch *cms, const char *key) {
  uint64_t *hashes = cms_get_hashes(cms, key);
  double num_add = cms_check_mean_min_alt(cms, hashes, cms->depth);
  free(hashes);
  return num_add;
}

static double cms_check_median_alt(CountMinSketch *cms, uint64_t *hashes,
                                   unsigned int num_hashes) {
  if (num_hashes < cms->depth) {
    fprintf(stderr, "Insufficient hashes to complete the median lookup of the "
                    "element to the count-min sketch!");
    return CMS_ERROR;
  }
  double num_add = 0;
  double *median_values = (double *)calloc(cms->depth, sizeof(double));
  for (unsigned int i = 0; i < cms->depth; ++i) {
    uint32_t bin = (hashes[i] % cms->width) + (i * cms->width);
    median_values[i] = cms->bins[bin];
  }
  // return the median of the median_values array... need to sort first
  qsort(median_values, cms->depth, sizeof(double), __double_compare);
  int32_t n = cms->depth;
  if (n % 2 == 0) {
    num_add = (median_values[n / 2] + median_values[n / 2 - 1]) / 2;
  } else {
    num_add = median_values[n / 2];
  }
  free(median_values);
  return num_add;
}

double cms_check_median(CountMinSketch *cms, const char *key) {
  uint64_t *hashes = cms_get_hashes(cms, key);
  // uint64_t *hashes = new_hashes(cms->hashes, key, cms->depth);

  double num_add = cms_check_median_alt(cms, hashes, cms->depth);
  free(hashes);
  return num_add;
}

uint64_t *cms_get_hashes_alt(CountMinSketch *cms, unsigned int num_hashes,
                             const char *key) {
  return cms->hash_function(num_hashes, key);
}

uint64_t *cms_get_hashes_fast(CountMinSketch *cms, const char *key) {

  for (unsigned int i = 0; i < cms->depth; ++i) {
    // cms->hashes[i] = __fnv_1a(key, i);
    cms->hashes[i] = XXH64(key, strlen(key), i);
  }
  return cms->hashes;
}

void multipleAll(CountMinSketch *cms, double by) {
  for (unsigned int i = 0; i < cms->depth; i++) {
    for (unsigned int j = 0; j < cms->width; j++) {
      cms->bins[i * cms->width + j] *= by;
    }
  }
}

// multiple all using AVX instructions
void multipleAllAVX(CountMinSketch *cms, double by) {
  __m256d by256 = _mm256_set1_pd(by);
  for (unsigned int i = 0; i < cms->depth; i++) {
    for (unsigned int j = 0; j < cms->width; j += 4) {
      __m256d bins256 = _mm256_loadu_pd(cms->bins + i * cms->width + j);
      bins256 = _mm256_mul_pd(bins256, by256);
      _mm256_storeu_pd(cms->bins + i * cms->width + j, bins256);
    }
  }
}

/*******************************************************************************
 *    PRIVATE FUNCTIONS
 *******************************************************************************/
static int __setup_cms(CountMinSketch *cms, unsigned int width,
                       unsigned int depth, double error_rate, double confidence,
                       cms_hash_function hash_function) {
  cms->width = width;
  cms->depth = depth;
  cms->confidence = confidence;
  cms->error_rate = error_rate;
  cms->elements_added = 0;
  cms->bins = (double *)calloc((width * depth), sizeof(double));
  cms->hash_function = (hash_function == NULL) ? __default_hash : hash_function;
  cms->hashes = (uint64_t *)calloc(depth, sizeof(uint64_t));

  if (NULL == cms->bins) {
    fprintf(stderr, "Failed to allocate %zu bytes for bins!",
            ((width * depth) * sizeof(double)));
    return CMS_ERROR;
  }
  return CMS_SUCCESS;
}

/* NOTE: The caller will free the results */
static uint64_t *__default_hash(unsigned int num_hashes, const char *str) {
  uint64_t *results = (uint64_t *)calloc(num_hashes, sizeof(uint64_t));
  for (unsigned int i = 0; i < num_hashes; ++i) {
    // results[i] = __fnv_1a(str, i);
    results[i] = XXH64(str, strlen(str), i);
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

static int __compare(const void *a, const void *b) {
  return (*(int64_t *)a - *(int64_t *)b);
}

static int __double_compare(const void *a, const void *b) {
  return (*(double *)a - *(double *)b);
}

#ifndef BARRUST_SIMPLE_COUNT_MIN_SKETCH_H__
#define BARRUST_SIMPLE_COUNT_MIN_SKETCH_H__

#ifdef __cplusplus
extern "C" {
#endif

#include <gsl/gsl_rng.h>
#include <stddef.h>
#include <stdint.h>

#define COUNT_MIN_SKETCH_VERSION "0.1.8"

/*  CMS_ERROR is problematic in that it is difficult to check for the error
    state since `INT_MIN` is a valid return value of the number of items
    inserted in at the furthest point
    TODO: Consider other options for signaling error states */
#define CMS_SUCCESS 0
#define CMS_ERROR INT32_MIN

/* https://gcc.gnu.org/onlinedocs/gcc/Alternate-Keywords.html#Alternate-Keywords
 */
#ifndef __GNUC__
#define inline
#endif

/* hashing function type */
typedef uint64_t *(*cms_hash_function)(unsigned int num_hashes,
                                       const char *key);
typedef uint64_t *(*ns_hash_function)(unsigned int num_hashes,
                                       const char *key);

typedef struct {
  uint32_t depth;
  uint32_t width;
  int64_t elements_added;
  double confidence;
  double error_rate;
  cms_hash_function hash_function;
  double *bins;
} CountMinSketch, count_min_sketch;

typedef struct {
  uint32_t depth;
  uint32_t width;
  int64_t elements_added;
  double confidence;
  double error_rate;
  ns_hash_function hash_function;
  double *bins;
  uint32_t row;
  gsl_rng *r;
} NitroSketch;

/*  Initialize the count-min sketch based on user defined width and depth
    Alternatively, one can also pass in a custom hash function

    Returns:
        CMS_SUCCESS
        CMS_ERROR   -   when unable to allocate the desired cms object or when
   width or depth are 0 */
int cms_init_alt(CountMinSketch *cms, unsigned int width, unsigned int depth,
                 cms_hash_function hash_function);
static __inline__ int cms_init(CountMinSketch *cms, unsigned int width,
                               unsigned int depth) {
  return cms_init_alt(cms, width, depth, NULL);
}

int ns_init_alt(NitroSketch *nts, unsigned int width, unsigned int depth,
                 cms_hash_function hash_function, gsl_rng *r);
static __inline__ int ns_init(NitroSketch *nts, unsigned int width,
                               unsigned int depth, gsl_rng *r) {
  return ns_init_alt(nts, width, depth, NULL, r);
}
/*  Initialize the count-min sketch based on user defined error rate and
    confidence values which is technically the optimal setup for the users needs
    Alternatively, one can also pass in a custom hash function

    Returns:
        CMS_SUCCESS
        CMS_ERROR   -   when unable to allocate the desired cms object or when
   error_rate or confidence is negative */
int cms_init_optimal_alt(CountMinSketch *cms, double error_rate,
                         double confidence, cms_hash_function hash_function);
static __inline__ int cms_init_optimal(CountMinSketch *cms, float error_rate,
                                       float confidence) {
  return cms_init_optimal_alt(cms, error_rate, confidence, NULL);
}

/*  Free all memory used in the count-min sketch

    Return:
        CMS_SUCCESS */
int cms_destroy(CountMinSketch *cms);

/*  Reset the count-min sketch to zero elements inserted

    Return:
        CMS_SUCCESS */
int cms_clear(CountMinSketch *cms);

/*  Insertion family of functions:

    Insert the provided key or hash values into the count-min sketch X number of
   times. Possible arguments: key         -   The key to insert x           -
   The number of times to insert the key; if this parameter is not present in
   the function then it is 1 hashes      -   A set of hashes that represent the
   key to insert; very useful when adding the same element to many count-min
                        sketches. This is only provieded if key is not.
        num_hashes  -   The number of hashes in the hash array
    Returns:
        On Success  -   The number of times `key` or `hashes` that have been
                        inserted using `min` estimation;
                        NOTE: result can be negative!
        On Failure  -   CMS_ERROR; this happens if there is an issue with the
                        number of hashes provided.
*/

/* Add the provided key to the count-min sketch `x` times */
void cms_add_inc(CountMinSketch *cms, const char *key, double x);
void cms_add_inc_alt(CountMinSketch *cms, uint64_t *hashes, double x);

/* Add the provided key to the count-min sketch */
static __inline__ void cms_add(CountMinSketch *cms, const char *key) {
  cms_add_inc(cms, key, 1.0);
}

void ns_add(NitroSketch *nts, const char *key, double x, double prob);

/* Determine the maximum number of times the key may have been inserted */
double cms_check(CountMinSketch *cms, const char *key);
double cms_check_alt(CountMinSketch *cms, uint64_t *hashes,
                     unsigned int num_hashes);

/*  Return the hashes for the provided key based on the hashing function of
    the count-min sketch
    NOTE: Useful when multiple count-min sketches use the same hashing
    functions
    NOTE: Up to the caller to free the array of hash values */
uint64_t *cms_get_hashes_alt(CountMinSketch *cms, unsigned int num_hashes,
                             const char *key);
uint64_t *ns_get_hashes_alt(NitroSketch *nts, unsigned int num_hashes,
                             const char *key);
static __inline__ uint64_t *cms_get_hashes(CountMinSketch *cms,
                                           const char *key) {
  return cms_get_hashes_alt(cms, cms->depth, key);
}

static __inline__ uint64_t *ns_get_hashes(NitroSketch *nts, const char *key) {
  return ns_get_hashes_alt(nts, nts->depth, key);
}

void multipleAll(CountMinSketch *cms, double by, int width, int depth);
double cms_check_median(CountMinSketch *cms, const char *key);
double ns_check_median(NitroSketch *nts, const char *key);
void geo_add(CountMinSketch *cms, const char *key, double x, double prob,
             gsl_rng *r);
void my_add(CountMinSketch *cms, const char *key, double x, double p);

/*  Determine the mean number of times the key may have been inserted
    NOTE: Mean check increases the over counting but is a `better` strategy
    when removes are added and negatives are possible */
double cms_check_mean(CountMinSketch *cms, const char *key);
double cms_check_mean_alt(CountMinSketch *cms, uint64_t *hashes,
                          unsigned int num_hashes);
double ns_check_mean(NitroSketch *ns, const char *key);

#ifdef __cplusplus
} // extern "C"
#endif

#endif

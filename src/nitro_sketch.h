#ifndef NITRO_SKETCH_H
#define NITRO_SKETCH_H

#include <gsl/gsl_rng.h>
#include <stdint.h>

/*  CMS_ERROR is problematic in that it is difficult to check for the error
    state since `INT_MIN` is a valid return value of the number of items
    inserted in at the furthest point
    TODO: Consider other options for signaling error states */
#define NS_SUCCESS 0
#define NS_ERROR INT32_MIN

/* https://gcc.gnu.org/onlinedocs/gcc/Alternate-Keywords.html#Alternate-Keywords
 */
#ifndef __GNUC__
#define __inline__ inline
#endif

/* hashing function type */
typedef uint64_t *(*ns_hash_function)(unsigned int num_hashes, const char *key);

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
  uint64_t *hashes;
  double *pool;
} NitroSketch;

/*  Initialize the count-min sketch based on user defined width and depth
    Alternatively, one can also pass in a custom hash function

    Returns:
        CMS_SUCCESS
        CMS_ERROR   -   when unable to allocate the desired cms object or when
   width or depth are 0 */

int ns_init_alt(NitroSketch *nts, unsigned int width, unsigned int depth,
                ns_hash_function hash_function, gsl_rng *r);
static __inline__ int ns_init(NitroSketch *nts, unsigned int width,
                              unsigned int depth, gsl_rng *r) {
  return ns_init_alt(nts, width, depth, NULL, r);
}
int ns_destroy(NitroSketch *ns);

void ns_add(NitroSketch *nts, const char *key, double x, double prob);

uint64_t *ns_get_hashes_alt(NitroSketch *nts, unsigned int num_hashes,
                            const char *key);

static __inline__ uint64_t *ns_get_hashes(NitroSketch *nts, const char *key) {
  return ns_get_hashes_alt(nts, nts->depth, key);
}

double ns_check_median(NitroSketch *nts, const char *key);

double ns_check_mean(NitroSketch *ns, const char *key);
uint64_t *ns_get_hashes_fast(NitroSketch *ns, const char *key);
double ns_check_mean_fast(NitroSketch *ns);
double ns_check_median_fast(NitroSketch *ns, const char *key);

#endif

#include "src/count_min_sketch.h"
#include <stdio.h>

int main() {

  CountMinSketch cms;
  int const width = 3;
  int const depth = 32;
  cms_init(&cms, width, depth);

  double res;
  for (int i = 0; i < 100; i++) {
    cms_add(&cms, "a");
  }

  for (int i = 0; i < 15; i++) {
    cms_add(&cms, "b");
  }

  for (int i = 0; i < 5; i++) {
    cms_add(&cms, "c");
  }

  for (int i = 0; i < 5; i++) {
    cms_add(&cms, "d");
  }
  printf("result: %lf\n", cms_check(&cms, "a"));
  printf("result: %lf\n", cms_check(&cms, "b"));
  printf("result: %lf\n", cms_check(&cms, "c"));
  printf("result: %lf\n", cms_check(&cms, "d"));
  cms_destroy(&cms);
}

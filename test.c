#include "src/count_min_sketch.h"
#include <stdio.h>

int main() {

  CountMinSketch cms;
  int const width = 100;
  int const depth = 2;
  cms_init(&cms, width, depth);

  int i, res;
  for (i = 0; i < 100; i++) {
    res = cms_add(&cms, "this is a test");
  }

  for (i = 0; i < 15; i++) {
    res = cms_add(&cms, "is a test");
  }

  res = cms_check(&cms, "this is a test");
  multipleAll(&cms, 0.5, width, depth);
  printf("result: %d\n", res);
  cms_destroy(&cms);
}

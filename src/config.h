#ifndef CONFIG_H
#define CONFIG_H

typedef struct {
  int src, dst, ts;
} Input;

typedef struct {
  int depth;
  int width;
} Config;

#endif

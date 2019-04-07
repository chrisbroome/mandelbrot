#ifndef UTILITIES_H
#define UTILITIES_H

#include <cstdint>

struct Color {
  union {
    struct {
      uint8_t r;
      uint8_t g;
      uint8_t b;
      uint8_t a;
    };
    int32_t rgba;
  };
};

#endif

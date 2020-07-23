//
// Created by tlou on 29.01.17.
//

#ifndef COMMONVISUALIZER_DATA_STRUCTURE_H
#define COMMONVISUALIZER_DATA_STRUCTURE_H

#include <cstdint>

namespace loco {
/**
 * length 4 vector (can be used as vec3)
 */
union Vec {
  float data[4];
  struct {
    float x;
    float y;
    float z;
    float w;
  };
  struct {
    float r;
    float g;
    float b;
    float a;
  };
};

union Vec8 {
  std::uint32_t uint32;
  std::int32_t int32;
  float float32;
  std::uint8_t uchar[4];
  struct {
    std::uint8_t r;
    std::uint8_t g;
    std::uint8_t b;
    std::uint8_t a;
  };
};

struct Transform {
  Vec translation;
  Vec rotation;
};

struct LightSource {
  Vec pos;
  Vec color;
};
}  // namespace loco
#endif  // COMMONVISUALIZER_DATA_STRUCTURE_H

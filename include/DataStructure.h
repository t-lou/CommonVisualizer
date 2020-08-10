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

#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wgnu-anonymous-struct"
// your code for which the warning gets suppressed
union Vec {
  float data[4];
  struct {
    float x{0.0f};
    float y{0.0f};
    float z{0.0f};
    float w{1.0f};
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
#pragma GCC diagnostic pop

struct Transform {
  Vec translation;
  Vec rotation;
};

struct LightSource {
  Vec pos{{0.0f, 0.0f, 0.0f, 1.0f}};
  Vec color{{1.0f, 1.0f, 1.0f, 1.0f}};
};
}  // namespace loco
#endif  // COMMONVISUALIZER_DATA_STRUCTURE_H

#ifndef COMMENVISUALIZER_DEPTH_MAP_SAVER_HPP
#define COMMENVISUALIZER_DEPTH_MAP_SAVER_HPP

#include <GL/glew.h>

#include <png++/image.hpp>
#include <png++/writer.hpp>

namespace loco {
class DepthMapSaver {
 public:
  static void saveImage(const std::uint32_t height, const std::uint32_t width,
                        const float distance, const std::string &filename) {
    png::image<png::rgb_pixel> image(width, height);

    std::vector<float> buffer(width * height, 0.0f);
    glReadPixels(0, 0, width, height, GL_DEPTH_COMPONENT, GL_FLOAT,
                 &buffer.front());

    std::size_t index = 0u;
    for (std::uint32_t r = 0; r < height; ++r) {
      for (std::uint32_t c = 0; c < width; ++c) {
        const float value = 1.0f - buffer.at(index);
        const float normalized = distance * value;
        const float clapped = std::min(1.0f, std::max(0.0f, normalized));
        const unsigned char pixel =
            static_cast<unsigned char>(255.0f * clapped);
        image.set_pixel(c, r, png::rgb_pixel(pixel, pixel, pixel));
        ++index;
      }
    }

    image.write(filename);
  }
};
}  // namespace loco

#endif

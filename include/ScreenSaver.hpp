#ifndef COMMENVISUALIZER_SCREEN_SAVER_HPP
#define COMMENVISUALIZER_SCREEN_SAVER_HPP

#include <GL/glew.h>

#include <png++/image.hpp>
#include <png++/writer.hpp>

namespace loco {
class ScreenSaver {
 public:
  static void saveImage(const std::uint32_t height, const std::uint32_t width,
                        const std::string &filename) {
    png::image<png::rgb_pixel> image(width, height);

    std::vector<unsigned char> buffer(width * height * 3u, 0);
    glReadPixels(0, 0, width, height, GL_BGR, GL_UNSIGNED_BYTE,
                 &buffer.front());

    std::size_t index = 0u;
    for (std::uint32_t r = 0; r < height; ++r) {
      for (std::uint32_t c = 0; c < width; ++c) {
        image.set_pixel(c, r,
                        png::rgb_pixel(buffer.at(index), buffer.at(index + 1u),
                                       buffer.at(index + 2u)));
        index += 3u;
      }
    }

    image.write(filename);
  }
};
}  // namespace loco

#endif

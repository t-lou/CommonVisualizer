#ifndef COMMENVISUALIZER_SCREEN_SAVER_HPP
#define COMMENVISUALIZER_SCREEN_SAVER_HPP

#include "include_gl.h"

#include <png++/image.hpp>
#include <png++/writer.hpp>

namespace loco
{
  class ScreenSaver
  {
  public:
    static void saveImage(const int height, const int width, 
                          const std::string &filename)
    {
      png::image<png::rgb_pixel> image(width, height);

      std::vector<unsigned char> buffer(width * height * 3, 0);
      glReadPixels(0, 0, width, height, GL_BGR, GL_UNSIGNED_BYTE, &buffer.front());

      int index = 0;
      for(int r = 0; r < height; ++r)
      {
        for(int c = 0; c < width; ++c)
        {
          image.set_pixel(c, r, png::rgb_pixel(buffer.at(index), buffer.at(index + 1), buffer.at(index + 2)));
          index += 3;
        }
      }

      image.write(filename);
    }
  };
}

#endif

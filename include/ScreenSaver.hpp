#ifndef COMMENVISUALIZER_SCREEN_SAVER_HPP
#define COMMENVISUALIZER_SCREEN_SAVER_HPP

#include "include_gl.h"

#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>

namespace loco
{
  class ScreenSaver
  {
  protected:

  public:
    ScreenSaver()
    {}

    ~ScreenSaver()
    {}

    static void saveImage(const int height, const int width, 
                          const std::string &filename)
    {
      cv::Mat canvas(height, width, CV_8UC3);
      std::vector<unsigned char> buffer(width * height * 3, 0);
      glReadPixels(0, 0, width, height, GL_BGR, GL_UNSIGNED_BYTE, &buffer.front());
      for(int r = 0; r < height; ++r)
      {
        for(int c = 0; c < width; ++c)
        {
          int index = (r * width + c) * 3;
          canvas.at<cv::Vec3b>(height - 1 - r, c) = cv::Vec3b(buffer.at(index),
                                                              buffer.at(index + 1),
                                                              buffer.at(index + 2));
        }
      }
      cv::imwrite(filename, canvas);
    }
  };
}

#endif

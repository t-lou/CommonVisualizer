/**
 * This program captures a scene with balls and save to /tmp.
 */

#include <cstdlib>
#include <iostream>
#include <vector>
#include <string>
#include <algorithm>
#include <functional>
#include "CommonVisualizer.hpp"
#include "include/ScreenSaver.hpp"

// how to compile (assuming all gl libraries installed)
// root directory of CommonVisualizer is /home/tlou/workspace/CommonVisualizer/ on my laptop
// clang++ test_screensaver.cpp -I/home/tlou/workspace/CommonVisualizer/ -lGL -lGLEW -lGLU -lglut -lglfw -lpng --std=c++11 -o test_screensaver

int main()
{
  std::vector<float> positions;
  std::vector<float> colors;
  loco::CommonVisualizer vis(600, 800, "sphere", loco::color::BLACK);
  loco::ScreenSaver screen_saver;
  if(vis.isInited())
  {
    constexpr float range{static_cast<float>(RAND_MAX)};
    // get (R-m)*r, where R is evenly distributed in [0, 1]
    auto gen_rand = [range](const float m, const float r) -> float
    {
      return (static_cast<float>(rand()) / range - m) * r;
    };

    constexpr int num {500};
    std::vector<float> positions(num * 3);
    std::vector<float> colors(num * 4);
    std::generate(positions.begin(), positions.end(), std::bind(gen_rand, 0.5f, 1000.0f));
    std::generate(colors.begin(), colors.end(), std::bind(gen_rand, 0.0f, 1.0f));

    vis.setDistance(1500.0f);
    vis.setLightSource(loco::Vec{0.0f, 0.0f, 1000.0f}, loco::color::WHITE);

    for(int count = 0; count < 20; ++count)
    {
      // random move the balls in 3D
      std::generate(positions.begin(), positions.end(),
                    [position = positions.begin(), gen_rand] () mutable -> float
                    { return *(position++) + gen_rand(0.5f, 10.0f); });

      vis.addPointCloud(positions, colors, 10.0f);

      if(!vis.playOnce())
      {
        break;
      }

      screen_saver.saveImage(600, 800, std::string{"/tmp/"} + std::to_string(count) + ".png");

      vis.resetScene();
    }
  }
  else
  {
    return 1;
  }
  return 0;
}

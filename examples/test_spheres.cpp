/**
 * This program creates thousands of spheres with Phong lighting which move randomly.
 */

#include <cstdlib>
#include <iostream>
#include <vector>
#include <algorithm>
#include <functional>
#include "CommonVisualizer.hpp"

// how to compile (assuming all gl libraries installed)
// root directory of CommonVisualizer is /home/tlou/workspace/CommonVisualizer/ on my laptop
// clang++ test_spheres.cpp -I/home/tlou/workspace/CommonVisualizer/ -lGL -lGLEW -lGLU -lglut -lglfw --std=c++11 -o test_spheres

int main()
{
  loco::CommonVisualizer vis(600, 800, "sphere", loco::color::BLACK);

  constexpr float range{static_cast<float>(RAND_MAX)};
  // get (R-m)*r, where R is evenly distributed in [0, 1]
  auto gen_rand = [range](const float m, const float r) -> float
  {
    return (static_cast<float>(rand()) / range - m) * r;
  };

  if(vis.isInited())
  {
    constexpr int num {500000};
    std::vector<float> positions(num * 3);
    std::vector<float> colors(num * 4);
    std::generate(positions.begin(), positions.end(), std::bind(gen_rand, 0.5f, 1000.0f));
    std::generate(colors.begin(), colors.end(), std::bind(gen_rand, 0.0f, 1.0f));

    vis.setDistance(1500.0f);
    vis.setLightSource(loco::Vec{0.0f, 0.0f, 1000.0f}, loco::color::WHITE);

    while(true)
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

      vis.resetScene();
    }
  }
  else
  {
    return 1;
  }
  return 0;
}

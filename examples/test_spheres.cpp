/**
 * This program creates thousands of spheres with Phong lighting which move randomly.
 */

#include <cstdlib>
#include <iostream>
#include <vector>
#include "CommonVisualizer.hpp"

// how to compile (assuming all gl libraries installed)
// root directory of CommonVisualizer is /home/tlou/workspace/CommonVisualizer/ on my laptop
// clang++ test_spheres.cpp -I/home/tlou/workspace/CommonVisualizer/ -lGL -lGLEW -lGLU -lglut -lglfw --std=c++11 -o test_spheres

int main()
{
  std::vector<float> positions;
  std::vector<float> colors;
  loco::CommonVisualizer vis(600, 800, "sphere", loco::color::BLACK);
  if(vis.isInited())
  {
    const int num = 500000;
    positions.reserve(num * 3);
    colors.reserve(num * 4);
    for(int i = 0; i < num; ++i)
    {
      positions.push_back((static_cast<float>(rand()) / (static_cast<float>(RAND_MAX)) - 0.5f) * 1000.0f);
      positions.push_back((static_cast<float>(rand()) / (static_cast<float>(RAND_MAX)) - 0.5f) * 1000.0f);
      positions.push_back((static_cast<float>(rand()) / (static_cast<float>(RAND_MAX)) - 0.5f) * 1000.0f);
      colors.push_back(static_cast<float>(rand()) / (static_cast<float>(RAND_MAX)));
      colors.push_back(static_cast<float>(rand()) / (static_cast<float>(RAND_MAX)));
      colors.push_back(static_cast<float>(rand()) / (static_cast<float>(RAND_MAX)));
      colors.push_back(static_cast<float>(rand()) / (static_cast<float>(RAND_MAX)));
      // colors.push_back(1.0f);
    }
    vis.setDistance(1500.0f);
    vis.setLightSource(loco::Vec{0.0f, 0.0f, 1000.0f}, loco::color::WHITE);

    while(true)
    {
      for(int i = 0; i < num; ++i)
      {
        float dx = (static_cast<float>(rand()) / (static_cast<float>(RAND_MAX)) - 0.5f) * 10.0f;
        float dy = (static_cast<float>(rand()) / (static_cast<float>(RAND_MAX)) - 0.5f) * 10.0f;
        float dz = (static_cast<float>(rand()) / (static_cast<float>(RAND_MAX)) - 0.5f) * 10.0f;
        positions.at(i * 3 + 0) += dx;
        positions.at(i * 3 + 1) += dy;
        positions.at(i * 3 + 2) += dz;
      }

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

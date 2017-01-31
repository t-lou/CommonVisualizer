#include <iostream>
#include "CommonVisualizer.hpp"

int main()
{
  loco::CommonVisualizer visualizer(600, 800, "gl viewer", loco::color::BLACK);
  if(visualizer.isInited())
  {
    visualizer.setLightSource(loco::Vec{0.0f, 0.0f, 10.0f}, loco::color::WHITE);
    visualizer.setDistance(3.0f);
    visualizer.setTransform(loco::Transform{loco::Vec{0.0f, 0.0f, 0.0f},
                                            loco::Vec{0.0f, 0.0f, 0.0f, 1.0f}});
//    visualizer.addCoordinateSign(loco::Transform{loco::Vec{0.0f, 0.0f, 0.0f},
//                                                 loco::Vec{0.0f, 0.0f, 0.0f, 1.0f}});
    float mesh[] = {-0.5f, -0.5f, 0.0f,
                    0.0f,  0.5f, 0.0f,
                    0.5f, -0.5f, 0.0f,
                    0.5f, 0.5f, 0.0f,
                    -0.5f, 0.5f, 0.0f,
                    0.0f,  -0.5f, 0.0f};
    float color[] = {1.0f, 0.0f, 0.0f, 1.0f,
                     0.0f, 1.0f, 0.0f, 1.0f,
                     0.0f, 0.0f, 1.0f, 1.0f,
                     1.0f, 1.0f, 0.0f, 1.0f,
                     1.0f, 0.0f, 1.0f, 1.0f,
                     0.0f, 1.0f, 1.0f, 1.0f,};
    float vertices[] = {};
    int indices[] = {};
//    visualizer.addMesh(std::vector<float>(mesh, mesh + 9), loco::color::WHITE);
//    visualizer.addMesh(std::vector<float>(mesh + 9, mesh + 18), loco::color::WHITE);
//    visualizer.addMesh(std::vector<float>(mesh, mesh + 18), std::vector<float>(color, color + 24));
//
    visualizer.addBox(loco::Transform{loco::Vec{0.0f, 0.0f, 0.0f}, loco::Vec{0.0f, 0.0f, 0.0f, 1.0f}},
                      loco::Vec{0.1f, 0.1f, 0.1f}, loco::color::WHITE);
    visualizer.addBox(loco::Transform{loco::Vec{0.0f, 0.0f, 0.3f}, loco::Vec{0.0f, 0.0f, 0.0f, 1.0f}},
                      loco::Vec{0.1f, 0.1f, 0.1f}, loco::color::WHITE);
    visualizer.addBox(loco::Transform{loco::Vec{0.0f, 0.0f, -0.3f}, loco::Vec{0.0f, 0.0f, 0.0f, 1.0f}},
                      loco::Vec{0.1f, 0.1f, 0.1f}, loco::color::WHITE);
//    visualizer.addPointCloud(std::vector<float>(mesh + 9, mesh + 18), loco::color::WHITE, 0.1f);
    visualizer.addPointCloud(std::vector<float>(mesh + 9, mesh + 18), std::vector<float>(color + 12, color + 24), 0.1f);
    visualizer.addPointCloud(std::vector<float>(mesh, mesh + 9), std::vector<float>(color, color + 12), 0.1f);

//    visualizer.addMesh(std::vector<float>(vertices, vertices + 3 * 1335),
//        std::vector<int>(indices, indices + 3 * 2452), loco::color::BLUE);

    float ends[] = {1.0f, 0.0f, 0.0f, -1.0f, 0.0f, 0.0f, };
    visualizer.addCylinder(std::vector<float>(ends, ends + 6), std::vector<float>(1, 0.1f),
                           std::vector<loco::Vec>(1, loco::color::WHITE));
    visualizer.play();
  }
  return 0;
}
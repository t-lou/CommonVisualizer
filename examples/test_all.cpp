#include <iostream>
#include <cstdlib>
#include <ctime>
#include <unistd.h>
#include <Eigen/Geometry>
#include "CommonVisualizer.hpp"

int main()
{
  loco::CommonVisualizer visualizer(600, 800, "gl viewer", loco::color::BLACK);
  if(visualizer.isInited())
  {
    visualizer.setLightSource(loco::Vec{0.0f, 0.0f, 10.0f}, loco::color::WHITE);
    visualizer.setDistance(3.0f);
    visualizer.setTransformCamera(loco::Transform{loco::Vec{0.0f, 0.0f, 0.0f},
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
    visualizer.addMesh(std::vector<float>(mesh, mesh + 18), std::vector<float>(color, color + 24));
//
    visualizer.addBox(loco::Transform{loco::Vec{0.0f, 0.0f, 0.0f}, loco::Vec{0.0f, 0.0f, 0.0f, 1.0f}},
                      loco::Vec{0.1f, 0.1f, 0.1f}, loco::color::WHITE);
    visualizer.addBox(loco::Transform{loco::Vec{0.0f, 0.0f, 0.3f}, loco::Vec{0.0f, 0.0f, 0.0f, 1.0f}},
                      loco::Vec{0.1f, 0.1f, 0.1f}, loco::color::WHITE);
    visualizer.addBox(loco::Transform{loco::Vec{0.0f, 0.0f, -0.3f}, loco::Vec{0.0f, 0.0f, 0.0f, 1.0f}},
                      loco::Vec{0.1f, 0.1f, 0.1f}, loco::color::WHITE);

    visualizer.addPointCloud(std::vector<float>(mesh + 9, mesh + 18), loco::color::WHITE, 0.1f);

//    visualizer.addPointCloud(std::vector<float>(mesh + 9, mesh + 18), std::vector<float>(color + 12, color + 24), 0.1f);
    visualizer.addPointCloud(std::vector<float>(mesh, mesh + 9), std::vector<float>(color, color + 12), 0.1f);

//    visualizer.addMesh(std::vector<float>(vertices, vertices + 3 * 1335),
//        std::vector<int>(indices, indices + 3 * 2452), loco::color::BLUE);

    float ends[] = {1.0f, 0.0f, 0.3f, 1.0f, 0.0f, -0.3f,
                    -1.0f, 0.0f, 0.3f, -1.0f, 0.0f, -0.3f};
    float ends_short[] = {1.0f, 0.0f, 0.1f, 1.0f, 0.0f, -0.1f,
                          -1.0f, 0.0f, 0.1f, -1.0f, 0.0f, -0.1f};
    visualizer.addCapsule(std::vector<float>(ends, ends + 12), std::vector<float>(2, 0.2f),
                          std::vector<loco::Vec>(2, loco::color::WHITE));
    visualizer.addCylinder(std::vector<float>(ends_short, ends_short + 12), std::vector<float>(2, 0.4f),
                           std::vector<loco::Vec>(2, loco::color::GREY));

    float ends_another[] = {0.0f, 1.0f, -0.5f, 0.0f, 1.0f, 0.5f};
    visualizer.addCone(std::vector<float>(ends_another, ends_another + 6), std::vector<float>(1, 0.2f),
                       std::vector<loco::Vec>(1, loco::color::WHITE));

    float ends_another_back[] = {0.0f, -1.0f, -0.5f, 0.0f, -1.0f, 0.5f};
    visualizer.addArrow(std::vector<float>(ends_another_back, ends_another_back + 6),
                        std::vector<float>(1, 0.3), std::vector<float>(1, 0.1),
                        std::vector<float>(1, 0.05), std::vector<loco::Vec>(1, loco::color::WHITE));

//    visualizer.play();
    int remaining = 0;
    loco::Vec rot{0.0f, 0.0f, 0.0f, 1.0f};
    Eigen::Matrix3f rot_current;
    Eigen::Matrix3f rot_delta;
    rot_current.setIdentity();
    rot_delta.setIdentity();
    while(visualizer.playOnce())
    {
      if(remaining <= 0)
      {
        remaining = rand() % 100 + 100;
        rot_delta = Eigen::AngleAxisf(((float)(rand() % 1000)) / 2e5f, Eigen::Vector3f(1,0,0)).matrix()
                    * Eigen::AngleAxisf(((float)(rand() % 1000)) / 2e5f, Eigen::Vector3f(0,1,0)).matrix()
                    * Eigen::AngleAxisf(((float)(rand() % 1000)) / 2e5f, Eigen::Vector3f(0,0,1)).matrix();
      }
      rot_current *= rot_delta;
      Eigen::Quaternionf rot_qua(rot_current);
      rot._x = rot_qua.x();
      rot._y = rot_qua.y();
      rot._z = rot_qua.z();
      rot._w = rot_qua.w();
      --remaining;

//      for(int i = 0; i < 10; ++i)
//      {
//        visualizer.setTransform(i, loco::Transform{loco::Vec{0.0f, 0.0f, 0.0f}, rot});
//      }
      visualizer.setTransform(loco::Transform{loco::Vec{0.0f, 0.0f, 0.0f}, rot});
      usleep(20);
    }
  }
  return 0;
}
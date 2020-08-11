
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wall"
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wextra"
#include <Eigen/Geometry>
#pragma GCC diagnostic pop
#pragma GCC diagnostic pop
#include <chrono>
#include <cmath>
#include <cstdlib>
#include <ctime>
#include <iostream>
#include <thread>

#include "CommonVisualizer.hpp"

int main() {
  loco::CommonVisualizer visualizer(600, 800, "gl viewer", loco::color::BLACK);
  if (visualizer.isInited()) {
    visualizer.setLightSource(loco::Vec{{0.0f, 0.0f, 10.0f}},
                              loco::color::WHITE);
    visualizer.setDistance(5.0f);
    visualizer.setTransformCamera(loco::Transform{
        loco::Vec{{0.0f, 0.0f, 0.0f}}, loco::Vec{{0.0f, 0.0f, 0.0f, 1.0f}}});
    float mesh[] = {-0.5f, -0.5f, 0.0f, 0.0f,  0.5f, 0.0f, 0.5f, -0.5f, 0.0f,
                    0.5f,  0.5f,  0.0f, -0.5f, 0.5f, 0.0f, 0.0f, -0.5f, 0.0f};
    float color[] = {
        1.0f, 0.0f, 0.0f, 1.0f, 0.0f, 1.0f, 0.0f, 1.0f, 0.0f, 0.0f, 1.0f, 1.0f,
        1.0f, 1.0f, 0.0f, 1.0f, 1.0f, 0.0f, 1.0f, 1.0f, 0.0f, 1.0f, 1.0f, 1.0f,
    };

    visualizer.addCoordinateSign(
        loco::Transform{loco::Vec{{1.0f, -1.0f, 0.0f}},
                        loco::Vec{{0.0f, 1.0f, 0.0f, 0.0f}}},
        0.3f);
    visualizer.addCoordinateSign(
        loco::Transform{loco::Vec{{1.0f, 1.0f, 0.0f}},
                        loco::Vec{{0.0f, 0.0f, 0.0f, 1.0f}}},
        0.3f);
    visualizer.addMesh(std::vector<float>(mesh, mesh + 18),
                       std::vector<float>(color, color + 24));
    visualizer.addBox(loco::Transform{loco::Vec{{0.0f, 0.0f, 0.3f}},
                                      loco::Vec{{0.0f, 0.0f, 0.0f, 1.0f}}},
                      loco::Vec{{0.1f, 0.1f, 0.1f}}, loco::color::WHITE);
    visualizer.addBox(loco::Transform{loco::Vec{{0.0f, 0.0f, -0.3f}},
                                      loco::Vec{{0.0f, 0.0f, 0.0f, 1.0f}}},
                      loco::Vec{{0.1f, 0.1f, 0.1f}}, loco::color::WHITE);

    visualizer.addPointCloud(std::vector<float>(mesh + 9, mesh + 18),
                             loco::color::WHITE, 0.1f);

    visualizer.addPointCloud(std::vector<float>(mesh, mesh + 9),
                             std::vector<float>(color, color + 12), 0.1f);

    float ends[] = {1.0f,  0.0f, 0.3f, 1.0f,  0.0f, -0.3f,
                    -1.0f, 0.0f, 0.3f, -1.0f, 0.0f, -0.3f};
    float ends_short[] = {1.0f,  0.0f, 0.1f, 1.0f,  0.0f, -0.1f,
                          -1.0f, 0.0f, 0.1f, -1.0f, 0.0f, -0.1f};
    visualizer.addCapsule(std::vector<float>(ends, ends + 12),
                          std::vector<float>(2, 0.2f),
                          std::vector<loco::Vec>(2, loco::color::WHITE));
    visualizer.addCylinder(std::vector<float>(ends_short, ends_short + 12),
                           std::vector<float>(2, 0.4f),
                           std::vector<loco::Vec>(2, loco::color::GREY));

    float ends_another[] = {0.0f, 1.0f, -0.5f, 0.0f, 1.0f, 0.5f};
    visualizer.addCone(std::vector<float>(ends_another, ends_another + 6),
                       std::vector<float>(1, 0.2f),
                       std::vector<loco::Vec>(1, loco::color::WHITE));

    float ends_another_back[] = {0.0f, -1.0f, -0.5f, 0.0f, -1.0f, 0.5f};
    visualizer.addArrow(
        std::vector<float>(ends_another_back, ends_another_back + 6),
        std::vector<float>(1, 0.3), std::vector<float>(1, 0.1),
        std::vector<float>(1, 0.05),
        std::vector<loco::Vec>(1, loco::color::WHITE));

    visualizer.setActiveWorld("line3d");

    std::vector<float> pos_line(3u * 3001u, 0.0f);
    for (std::size_t i{0}; i <= 3000u; ++i) {
      pos_line[3u * i] = std::sin(0.01f * (float)i) * 0.5f;
      pos_line[3u * i + 1u] = std::cos(0.01f * (float)i) * 0.5f;
      pos_line[3u * i + 2u] = 1.0f - 0.0003f * (float)i;
    }
    visualizer.addLine(pos_line, 0.03f, loco::color::RED);

    int remaining = 0;
    Eigen::Matrix3f orientation{Eigen::Matrix3f::Identity()};
    Eigen::Matrix3f rot_delta{Eigen::Matrix3f::Identity()};
    while (visualizer.playOnce()) {
      if (remaining <= 0) {
        remaining = rand() % 100 + 100;
        rot_delta = Eigen::AngleAxisf(((float)(rand() % 1000)) / 2e5f,
                                      Eigen::Vector3f(1, 0, 0))
                        .matrix() *
                    Eigen::AngleAxisf(((float)(rand() % 1000)) / 2e5f,
                                      Eigen::Vector3f(0, 1, 0))
                        .matrix() *
                    Eigen::AngleAxisf(((float)(rand() % 1000)) / 2e5f,
                                      Eigen::Vector3f(0, 0, 1))
                        .matrix();
      }
      orientation *= rot_delta;
      const Eigen::Quaternionf rot_qua{orientation};

      visualizer.setTransform(loco::Transform{
          loco::Vec{{0.0f, 0.0f, 0.0f}},
          loco::Vec{{rot_qua.x(), rot_qua.y(), rot_qua.z(), rot_qua.w()}}});
      std::this_thread::sleep_for(std::chrono::milliseconds(20));
      --remaining;
    }
  }
  return 0;
}
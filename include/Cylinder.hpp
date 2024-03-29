//
// Created by tlou on 01.02.17.
//

#ifndef COMMONVISUALIZER_CYLINDER_HPP
#define COMMONVISUALIZER_CYLINDER_HPP

#include "CylinderSide.hpp"
#include "Object.hpp"
#include "OrientedCircle.hpp"

namespace loco {
class Cylinder : public Container {
 public:
  Cylinder(const std::vector<float> &positions,
           const std::vector<float> &radius, const std::vector<Vec> &colors,
           const GLuint id_program_side, const GLuint id_program_end)
      : Container() {
    assert(positions.size() % 6 == 0);
    assert(positions.size() / 6 == radius.size());
    assert(radius.size() == colors.size());

    _belongings.reserve(2 * colors.size());

    std::vector<Vec> colors_;
    std::vector<float> radius_;
    std::vector<float> normals_;

    colors_.reserve(colors.size() * 2);
    radius_.reserve(radius.size() * 2);
    normals_.reserve(positions.size());

    for (size_t id = 0u; id < colors.size(); ++id) {
      const size_t id6 = id * 6u;
      glm::vec3 to =
          glm::normalize(glm::vec3(positions.at(id6 + 3), positions.at(id6 + 4),
                                   positions.at(id6 + 5)) -
                         glm::vec3(positions.at(id6), positions.at(id6 + 1),
                                   positions.at(id6 + 2)));
      for (std::size_t i = 0u; i < 3u; ++i) {
        normals_.push_back(-to[i]);
      }
      for (std::size_t i = 0u; i < 3u; ++i) {
        normals_.push_back(to[i]);
      }
      colors_.push_back(colors.at(id));
      colors_.push_back(colors.at(id));
      radius_.push_back(radius.at(id));
      radius_.push_back(radius.at(id));
    }

    addObject(std::make_unique<OrientedCircle>(positions, normals_, radius_,
                                               colors_, id_program_end));
    addObject(std::make_unique<CylinderSide>(positions, radius, colors,
                                             id_program_side));
  }
};
}  // namespace loco

#endif  // COMMONVISUALIZER_CYLINDER_HPP

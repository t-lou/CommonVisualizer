//
// Created by tlou on 22.02.17.
//

#ifndef COMMONVISUALIZER_CONE_HPP
#define COMMONVISUALIZER_CONE_HPP

#include "ConeSide.hpp"
#include "Container.hpp"
#include "OrientedCircle.hpp"

namespace loco {
class Cone : public Container {
 public:
  Cone(const std::vector<float> &positions, const std::vector<float> &radius,
       const std::vector<Vec> &colors, const GLuint id_program_side,
       const GLuint id_program_bottom)
      : Container() {
    assert(positions.size() == radius.size() * 6u);
    assert(colors.size() == radius.size());

    _belongings.reserve(2u);
    addObject(
        std::make_unique<ConeSide>(positions, radius, colors, id_program_side));

    // bottoms
    std::vector<float> normals;
    std::vector<float> pos_bottom;
    normals.reserve(radius.size() * 3u);
    pos_bottom.reserve(radius.size() * 3u);
    for (std::size_t id = 0u; id < radius.size(); ++id) {
      const std::size_t id6 = id * 6u;
      glm::vec3 to =
          glm::normalize(glm::vec3(positions.at(id6 + 3u), positions.at(id6 + 4u),
                                   positions.at(id6 + 5u)) -
                         glm::vec3(positions.at(id6), positions.at(id6 + 1u),
                                   positions.at(id6 + 2u)));
      for (std::size_t i = 0u; i < 3u; ++i) {
        normals.push_back(-to[i]);
        pos_bottom.push_back(positions.at(id6 + i));
      }
    }
    addObject(std::make_unique<OrientedCircle>(pos_bottom, normals, radius,
                                               colors, id_program_bottom));
  }
};
}  // namespace loco

#endif  // COMMONVISUALIZER_CONE_HPP

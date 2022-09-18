//
// Created by tlou on 01.02.17.
//

#ifndef COMMONVISUALIZER_CAPSULE_HPP
#define COMMONVISUALIZER_CAPSULE_HPP

#include "CloudSphereUnicolor.hpp"
#include "CylinderSide.hpp"
#include "Object.hpp"

namespace loco {
class Capsule : public Container {
 public:
  /**
   * constructor for an array of Capsules
   * @param positions vector of positions of length 6*num, for each arrow:
   *        from_x, from_y, from_z, to_x, to_y, to_z
   * @param radius
   * @param colors
   * @param id_program_side
   * @param id_program_end
   */
  Capsule(const std::vector<float> &positions, const std::vector<float> &radius,
          const std::vector<Vec> &colors, const GLuint id_program_side,
          const GLuint id_program_end)
      : Container() {
    assert(positions.size() % 6u == 0u);
    assert(positions.size() / 6u == radius.size());
    assert(radius.size() == colors.size());

    _belongings.reserve(2u * colors.size());

    for (std::size_t id = 0u; id < colors.size(); ++id) {
      const std::size_t id6 = id * 6u;
      addObject(std::make_unique<CloudSphereUnicolor>(
          std::vector<float>(positions.begin() + id6,
                             positions.begin() + id6 + 6u),
          colors.at(id), radius.at(id), id_program_end));
      addObject(std::make_unique<CylinderSide>(
          std::vector<float>(positions.begin() + id6,
                             positions.begin() + id6 + 6u),
          std::vector<float>(1u, radius.at(id)),
          std::vector<Vec>(1u, colors.at(id)), id_program_side));
    }
  }
};
}  // namespace loco

#endif  // COMMONVISUALIZER_CAPSULE_HPP

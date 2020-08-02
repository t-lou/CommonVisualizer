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
    assert(positions.size() % 6 == 0);
    assert(positions.size() / 6 == radius.size());
    assert(radius.size() == colors.size());

    _belongings.reserve(2 * colors.size());

    for (size_t id = 0; id < colors.size(); ++id) {
      const size_t id6 = id * 6;
      addObject(std::make_unique<CloudSphereUnicolor>(
          std::vector<float>(positions.begin() + id6,
                             positions.begin() + id6 + 6),
          colors.at(id), radius.at(id), id_program_end));
      addObject(std::make_unique<CylinderSide>(
          std::vector<float>(positions.begin() + id6,
                             positions.begin() + id6 + 6),
          std::vector<float>(1, radius.at(id)),
          std::vector<Vec>(1, colors.at(id)), id_program_side));
    }
  }
};
}  // namespace loco

#endif  // COMMONVISUALIZER_CAPSULE_HPP

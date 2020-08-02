#ifndef COMMONVISUALIZER_LINE_HPP
#define COMMONVISUALIZER_LINE_HPP

#include "Capsule.hpp"
#include "CloudSphereUnicolor.hpp"
#include "CylinderSide.hpp"

namespace loco {
class Line : public Container {
 public:
  /**
   * constructor for one line constructed with capsules
   * @param positions vector of positions: x0,y0,z0,x1,y1,z1,...
   * @param radius
   * @param colors
   * @param id_program_side
   * @param id_program_end
   */
  Line(const std::vector<float> &positions, const float radius,
       const Vec &color, const GLuint id_program_side,
       const GLuint id_program_end)
      : Container() {
    assert(positions.size() % 3u == 0u && positions.size() >= 6u);
    const std::size_t num{positions.size() / 3u};

    _belongings.reserve(num);

    addObject(std::make_unique<CloudSphereUnicolor>(positions, color, radius,
                                                    id_program_end));
    for (size_t id = 1u; id < num; ++id) {
      addObject(std::make_unique<CylinderSide>(
          std::vector<float>(positions.begin() + id * 3u - 3u,
                             positions.begin() + id * 3u + 3u),
          std::vector<float>(1, radius), std::vector<Vec>(1, color),
          id_program_side));
    }
  }
};
}  // namespace loco

#endif  // COMMONVISUALIZER_LINE_HPP

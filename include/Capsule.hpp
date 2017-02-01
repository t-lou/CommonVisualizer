//
// Created by tlou on 01.02.17.
//

#ifndef COMMONVISUALIZER_CAPSULE_HPP
#define COMMONVISUALIZER_CAPSULE_HPP

#include "Object.hpp"
#include "CloudSphereUnicolor.hpp"
#include "CylinderSide.hpp"

namespace loco
{
  class Capsule : public Container
  {
  protected:

  public:
    Capsule(const std::vector<float> &positions, const std::vector<float> &radius, const std::vector<Vec> &colors,
            const GLuint id_program_side, const GLuint id_program_end) :
        Container()
    {
      assert(positions.size() % 6 == 0);
      assert(positions.size() / 6 == radius.size());
      assert(radius.size() == colors.size());

      _belongings.reserve(2 * colors.size());

      for(size_t id = 0; id < colors.size(); ++id)
      {
        const size_t id6 = id * 6;
        addObject(new CloudSphereUnicolor(
            std::vector<float>(positions.begin() + id6, positions.begin() + id6 + 6),
            colors.at(id), radius.at(id), id_program_end));
        addObject(new CylinderSide(
            std::vector<float>(positions.begin() + id6, positions.begin() + id6 + 6),
            std::vector<float>(1, radius.at(id)),
            std::vector<Vec>(1, colors.at(id)), id_program_side));
      }
    }

    ~Capsule()
    {}
  };
}

#endif //COMMONVISUALIZER_CAPSULE_HPP

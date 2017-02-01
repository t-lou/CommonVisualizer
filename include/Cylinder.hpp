//
// Created by tlou on 01.02.17.
//

#ifndef COMMONVISUALIZER_CYLINDER_HPP
#define COMMONVISUALIZER_CYLINDER_HPP


#include "Object.hpp"
#include "OrientedCircle.hpp"
#include "CylinderSide.hpp"

namespace loco
{
  class Cylinder : public Container
  {
  protected:

  public:
    Cylinder(const std::vector<float> &positions, const std::vector<float> &radius, const std::vector<Vec> &colors,
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
        glm::vec3 to = glm::normalize(glm::vec3(positions.at(id6 + 3) - positions.at(id6),
                                                positions.at(id6 + 4) - positions.at(id6 + 1),
                                                positions.at(id6 + 5) - positions.at(id6 + 2)));
        std::vector<float> normal_end;
        normal_end.resize(6);

        for(int i = 0; i < 3; ++i)
        {
          normal_end.at(i) = -to[i];
          normal_end.at(i + 3) = to[i];
        }

        addObject(new OrientedCircle(
            std::vector<float>(positions.begin() + id6, positions.begin() + id6 + 6), normal_end,
            std::vector<float>(2, radius.at(id)), std::vector<Vec>(2, colors.at(id)), id_program_end));
        addObject(new CylinderSide(
            std::vector<float>(positions.begin() + id6, positions.begin() + id6 + 6),
            std::vector<float>(1, radius.at(id)),
            std::vector<Vec>(1, colors.at(id)), id_program_side));
      }
    }

    ~Cylinder()
    {}
  };
}

#endif //COMMONVISUALIZER_CYLINDER_HPP

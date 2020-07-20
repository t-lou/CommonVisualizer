//
// Created by tlou on 22.02.17.
//

#ifndef COMMONVISUALIZER_CONE_HPP
#define COMMONVISUALIZER_CONE_HPP

#include "Container.hpp"
#include "ConeSide.hpp"
#include "OrientedCircle.hpp"

namespace loco
{
  class Cone : public Container
  {
  protected:

  public:
    Cone(const std::vector<float> &positions, const std::vector<float> &radius,
         const std::vector<Vec> &colors, const GLuint id_program_side, const GLuint id_program_bottom) :
        Container()
    {
      assert(positions.size() == radius.size() * 6);
      assert(colors.size() == radius.size());

      _belongings.reserve(2);
      addObject(new ConeSide(positions, radius, colors, id_program_side));

      // bottoms
      std::vector<float> normals;
      std::vector<float> pos_bottom;
      normals.reserve(radius.size() * 3);
      pos_bottom.reserve(radius.size() * 3);
      for(size_t id = 0; id < radius.size(); ++id)
      {
        const size_t id6 = id * 6;
        glm::vec3 to = glm::normalize(glm::vec3(positions.at(id6 + 3), positions.at(id6 + 4), positions.at(id6 + 5))
                                      - glm::vec3(positions.at(id6), positions.at(id6 + 1), positions.at(id6 + 2)));
        for(int i = 0; i < 3; ++i)
        {
          normals.push_back(-to[i]);
          pos_bottom.push_back(positions.at(id6 + i));
        }
      }
      addObject(new OrientedCircle(pos_bottom, normals, radius, colors, id_program_bottom));
    }
  };
}

#endif //COMMONVISUALIZER_CONE_HPP

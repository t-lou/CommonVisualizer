//
// Created by tlou on 23.02.17.
//

#ifndef COMMONVISUALIZER_ARROW_HPP
#define COMMONVISUALIZER_ARROW_HPP

#include "Container.hpp"
#include "ConeSide.hpp"
#include "CylinderSide.hpp"
#include "OrientedCircle.hpp"

namespace loco
{
  class Arrow : public Container
  {
  protected:

  public:
    Arrow(const std::vector<float> &positions,
          const std::vector<float> &radius_head, const std::vector<float> &radius_body,
          const std::vector<float> &length_head, const std::vector<Vec> &colors,
          const GLuint id_program_cylinder, const GLuint id_program_cone, const GLuint id_program_circle)
    {
      assert(positions.size() == radius_body.size() * 6);
      assert(radius_body.size() == radius_head.size());
      assert(radius_body.size() == length_head.size());
      assert(radius_body.size() == colors.size());

      _belongings.reserve(4);

      std::vector<float> pos_head;
      std::vector<float> pos_body;
      std::vector<float> pos_bottom;
      std::vector<float> normals;
      std::vector<float> pos_link;
      pos_head.reserve(positions.size());
      pos_body.reserve(positions.size());
      normals.reserve(positions.size() / 2);
      pos_link.reserve(positions.size() / 2);
      pos_bottom.reserve(positions.size() / 2);
      for(size_t id = 0; id < radius_body.size(); ++id)
      {
        const size_t id6 = id * 6;
        glm::vec3 to = glm::normalize(glm::vec3(positions.at(id6 + 3), positions.at(id6 + 4), positions.at(id6 + 5))
                                      - glm::vec3(positions.at(id6), positions.at(id6 + 1), positions.at(id6 + 2)));
        float len = glm::length(to);
        glm::vec3 link = glm::vec3(positions.at(id6 + 3), positions.at(id6 + 4), positions.at(id6 + 5))
                         - to * length_head.at(id);
        pos_head.push_back(link[0]);
        pos_head.push_back(link[1]);
        pos_head.push_back(link[2]);
        pos_head.insert(pos_head.end(), positions.begin() + id6 + 3, positions.begin() + id6 + 6);

        pos_body.push_back(link[0]);
        pos_body.push_back(link[1]);
        pos_body.push_back(link[2]);
        pos_body.insert(pos_body.end(), positions.begin() + id6, positions.begin() + id6 + 3);

        pos_bottom.insert(pos_bottom.end(), positions.begin() + id6, positions.begin() + id6 + 3);

        pos_link.push_back(link[0]);
        pos_link.push_back(link[1]);
        pos_link.push_back(link[2]);

        normals.push_back(-to[0]);
        normals.push_back(-to[1]);
        normals.push_back(-to[2]);
      }

      addObject(new ConeSide(pos_head, radius_head, colors, id_program_cone));
      addObject(new CylinderSide(pos_body, radius_body, colors, id_program_cylinder));
      addObject(new OrientedCircle(pos_link, normals, radius_head, colors, id_program_circle));
      addObject(new OrientedCircle(pos_bottom, normals, radius_body, colors, id_program_circle));
    }

    ~Arrow()
    {}
  };
}


#endif //COMMONVISUALIZER_ARROW_HPP

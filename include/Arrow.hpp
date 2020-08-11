//
// Created by tlou on 23.02.17.
//

#ifndef COMMONVISUALIZER_ARROW_HPP
#define COMMONVISUALIZER_ARROW_HPP

#include "ConeSide.hpp"
#include "Container.hpp"
#include "CylinderSide.hpp"
#include "OrientedCircle.hpp"

namespace loco {
class Arrow : public Container {
 protected:
 public:
  /**
   * Constructor for arrow array of size num
   * @param positions vector of positions of length 6*num, for each arrow:
   *        from_x, from_y, from_z, to_x, to_y, to_z
   * @param radius_head
   * @param radius_body
   * @param length_head
   * @param colors
   * @param id_program_cylinder
   * @param id_program_cone
   * @param id_program_circle
   */
  Arrow(const std::vector<float> &positions,
        const std::vector<float> &radius_head,
        const std::vector<float> &radius_body,
        const std::vector<float> &length_head, const std::vector<Vec> &colors,
        const GLuint id_program_cylinder, const GLuint id_program_cone,
        const GLuint id_program_circle) {
    assert(positions.size() == radius_body.size() * 6u);
    assert(radius_body.size() == radius_head.size());
    assert(radius_body.size() == length_head.size());
    assert(radius_body.size() == colors.size());

    _belongings.reserve(4);

    auto append_glm_vec3{
        [](const glm::vec3 &in, std::vector<float> &out) -> void {
          for (int i : {0, 1, 2}) {
            out.push_back(in[i]);
          }
        }};

    // convert the positions of arrow to cone and cylinder, and compute the
    // direction
    const std::size_t num{length_head.size()};
    std::vector<float> pos_head;
    std::vector<float> pos_body;
    std::vector<float> pos_bottom;
    std::vector<float> normals;
    std::vector<float> pos_link;
    pos_head.reserve(num * 6u);
    pos_body.reserve(num * 6u);
    normals.reserve(num / 3u);
    pos_link.reserve(num / 3u);
    pos_bottom.reserve(num / 3u);
    for (std::size_t id = 0; id < num; ++id) {
      const std::size_t id6{id * 6u};
      const glm::vec3 root{positions.at(id6), positions.at(id6 + 1),
                           positions.at(id6 + 2)};
      const glm::vec3 point{positions.at(id6 + 3), positions.at(id6 + 4),
                            positions.at(id6 + 5)};
      const glm::vec3 to{glm::normalize(point - root)};
      const glm::vec3 link{point - to * length_head.at(id)};
      append_glm_vec3(link, pos_head);
      append_glm_vec3(point, pos_head);

      append_glm_vec3(link, pos_body);
      append_glm_vec3(root, pos_body);

      append_glm_vec3(root, pos_bottom);

      append_glm_vec3(link, pos_link);

      append_glm_vec3(-to, normals);
    }

    addObject(std::make_unique<ConeSide>(pos_head, radius_head, colors,
                                         id_program_cone));
    addObject(std::make_unique<CylinderSide>(pos_body, radius_body, colors,
                                             id_program_cylinder));
    addObject(std::make_unique<OrientedCircle>(pos_link, normals, radius_head,
                                               colors, id_program_circle));
    addObject(std::make_unique<OrientedCircle>(pos_bottom, normals, radius_body,
                                               colors, id_program_circle));
  }
};
}  // namespace loco

#endif  // COMMONVISUALIZER_ARROW_HPP

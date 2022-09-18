//
// Created by tlou on 30.01.17.
//

#ifndef COMMONVISUALIZER_BOX_HPP
#define COMMONVISUALIZER_BOX_HPP

#include "MeshUnicolor.hpp"

namespace loco {
class Box : public MeshUnicolor {
 protected:
  /**
   * create a vector of point positions of corners for drawing. it is shit but
   * it works
   * @param transform
   * @param scale
   * @return
   */
  std::vector<float> getVertex(const Transform &transform, const Vec &scale) {
    glm::mat4 tf_mat = Object::transformToMat4(transform);
    std::vector<float> corners;
    std::vector<float> vertex;
    corners.reserve(24u);
    vertex.reserve(108u);

    // get transformed corners
    for (std::int32_t id = 0; id < 8; ++id) {
      glm::vec4 corner(scale.xyzw.x, scale.xyzw.y, scale.xyzw.z, 1.0f);
      if (id >= 4) {
        corner.z = -corner.z;
      }
      if (id & 2) {
        corner.x = -corner.x;
      }
      if (id % 2 == 0) {
        corner.y = -corner.y;
      }

      corner = tf_mat * corner;

      for (std::size_t i = 0u; i < 3u; ++i) {
        corners.push_back(corner[i]);
      }
    }

    // triangulate mesh
    const std::vector<float>::const_iterator begin = corners.begin();
    auto append_corners{[&vertex, begin](const std::size_t id) -> void {
      vertex.insert(vertex.end(), begin + id * 3u, begin + id * 3u + 3u);
    }};

    for (const std::size_t id : {0u, 1u, 2u, 2u, 1u, 3u}) {
      append_corners(id);
    }
    for (const std::size_t id : {4u, 6u, 5u, 5u, 6u, 7u}) {
      append_corners(id);
    }
    for (const std::size_t id : {0u, 5u, 1u, 4u, 5u, 0u}) {
      append_corners(id);
    }
    for (const std::size_t id : {2u, 3u, 6u, 7u, 6u, 3u}) {
      append_corners(id);
    }
    for (const std::size_t id : {0u, 2u, 6u, 6u, 4u, 0u}) {
      append_corners(id);
    }
    for (const std::size_t id : {3u, 1u, 7u, 7u, 1u, 5u}) {
      append_corners(id);
    }

    return vertex;
  }

 public:
  /**
   * constructor for single blx
   * @param transform
   * @param scale
   * @param color
   * @param id_program
   */
  Box(const Transform &transform, const Vec &scale, const Vec &color,
      const GLuint id_program)
      : MeshUnicolor(getVertex(transform, scale), color, id_program) {}
};
}  // namespace loco

#endif  // COMMONVISUALIZER_BOX_HPP

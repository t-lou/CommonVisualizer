#ifndef COMMONVISUALIZER_COORDINATE_UNITS_3D_HPP
#define COMMONVISUALIZER_COORDINATE_UNITS_3D_HPP

#include "Arrow.hpp"
#include "Container.hpp"

namespace loco {
class CoordinateUnits3D : public Container {
 public:
  CoordinateUnits3D(const Transform &transform, const float scale,
                    const GLuint id_program_cylinder,
                    const GLuint id_program_cone,
                    const GLuint id_program_circle) {
    const glm::mat4 tf_mat{Object::transformToMat4(transform)};
    auto apply_transform{[&tf_mat](std::vector<float> &pos) -> void {
      glm::vec4 pos_vec{pos[0u], pos[1u], pos[2u], 1.0f};
      pos_vec = tf_mat * pos_vec;
      for (std::int32_t i = 0; i < 3; ++i) {
        pos[i] = pos_vec[i];
      }
    }};

    const float radius_body{scale / 30.0f};
    const float radius_head{radius_body * 2.0f};
    const float length_head{radius_head * 3.0f};
    const std::vector<Vec> colors{Vec{{1.0f, 0.0f, 0.0f, 1.0f}},
                                  Vec{{0.0f, 1.0f, 0.0f, 1.0f}},
                                  Vec{{0.0f, 0.0f, 1.0f, 1.0f}}};
    std::vector<float> positions;
    positions.reserve(6u * 3u);
    // add ends
    for (std::size_t id_dim = 0u; id_dim < 3u; ++id_dim) {
      std::vector<float> color{0.0f, 0.0f, 0.0f, 1.0f};
      std::vector<float> start(3u, 0.0f);
      std::vector<float> end(3u, 0.0f);
      end[id_dim] = scale;
      color[id_dim] = 1.0f;

      apply_transform(start);
      apply_transform(end);
      positions.insert(positions.end(), start.begin(), start.end());
      positions.insert(positions.end(), end.begin(), end.end());
    }

    addObject(std::make_unique<Arrow>(
        positions, std::vector<float>(3, radius_head),
        std::vector<float>(3u, radius_body), std::vector<float>(3u, length_head),
        colors, id_program_cylinder, id_program_cone, id_program_circle));
  }
};
}  // namespace loco

#endif  // COMMONVISUALIZER_COORDINATE_UNITS_3D_HPP

//
// Created by tlou on 22.02.17.
//

#ifndef COMMONVISUALIZER_CONESIDE_HPP
#define COMMONVISUALIZER_CONESIDE_HPP

#include <vector>

#include "CylinderSide.hpp"

namespace loco {
class ConeSide : public CylinderSide {
  using CylinderSide::display;

 protected:
  GLuint _buffer_is_convex;
  GLuint _buffer_end;

 public:
  ConeSide(const std::vector<float> &positions,
           const std::vector<float> &radius, const std::vector<float> &ends,
           const std::vector<bool> &is_convex, const std::vector<Vec> &colors,
           const GLuint id_program)
      : CylinderSide(positions, radius, colors, id_program) {
    assert(radius.size() == is_convex.size());
    assert(radius.size() == ends.size());

    // buffer for ends
    genBufferVectorFloat(ends, _buffer_end);

    // buffer for whether the shapes represent convex shapes
    const bool &begin = is_convex.front();
    glGenBuffers(1, &_buffer_is_convex);
    glBindBuffer(GL_ARRAY_BUFFER, _buffer_is_convex);
    glBufferData(GL_ARRAY_BUFFER, is_convex.size(), &begin,
                 GL_STATIC_DRAW);  // std::vector<bool>.front()...
  }

  ConeSide(const std::vector<float> &positions,
           const std::vector<float> &radius, const std::vector<bool> &is_convex,
           const std::vector<Vec> &colors, const GLuint id_program)
      : ConeSide(positions, radius, std::vector<float>(radius.size(), 0.0f),
                 is_convex, colors, id_program) {}

  ConeSide(const std::vector<float> &positions,
           const std::vector<float> &radius, const std::vector<float> &ends,
           const std::vector<Vec> &colors, const GLuint id_program)
      : ConeSide(positions, radius, ends,
                 std::vector<bool>(radius.size(), true), colors, id_program) {}

  ConeSide(const std::vector<float> &positions,
           const std::vector<float> &radius, const std::vector<Vec> &colors,
           const GLuint id_program)
      : ConeSide(positions, radius, std::vector<float>(radius.size(), 0.0f),
                 std::vector<bool>(radius.size(), true), colors, id_program) {}

  ~ConeSide() {
    glDeleteBuffers(1, (GLuint *)&_buffer_position);
    glDeleteBuffers(1, (GLuint *)&_buffer_color);
    glDeleteBuffers(1, (GLuint *)&_buffer_radius);
    glDeleteBuffers(1, (GLuint *)&_buffer_is_convex);
    glDeleteBuffers(1, (GLuint *)&_buffer_end);
    glDeleteVertexArrays(1, &_id_array);
  }

  void display(const glm::mat4 &proj) { CylinderSide::display(proj); }
};
}  // namespace loco

#endif  // COMMONVISUALIZER_CONESIDE_HPP

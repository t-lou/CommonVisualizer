//
// Created by tlou on 31.01.17.
//

#ifndef COMMONVISUALIZER_CYLINDERSIDE_HPP
#define COMMONVISUALIZER_CYLINDERSIDE_HPP

#include "Object.hpp"

namespace loco {
class CylinderSide : public Object {
 protected:
  GLuint _buffer_color;
  GLuint _buffer_radius;

  void generateBuffer(const std::vector<float> &positions,
                      const std::vector<float> &radius,
                      const std::vector<Vec> &colors) {
    std::vector<float> radius_;
    std::vector<float> colors_;
    radius_.reserve(radius.size() * 2u);
    colors_.reserve(radius.size() * 8u);
    for (const float &r : radius) {
      radius_.push_back(r);
      radius_.push_back(r);
    }
    for (const Vec &color : colors) {
      colors_.insert(colors_.end(), color._data, color._data + 4);
      colors_.insert(colors_.end(), color._data, color._data + 4);
    }
    genBufferVectorFloat(positions, _buffer_position);
    genBufferVectorFloat(radius_, _buffer_radius);
    genBufferVectorFloat(colors_, _buffer_color);
  }

 public:
  CylinderSide(const std::vector<float> &positions,
               const std::vector<float> &radius, const std::vector<Vec> &colors,
               const GLuint id_program)
      : Object(id_program) {
    assert(positions.size() % 6u == 0u);
    assert(positions.size() / 6u == radius.size());
    assert(radius.size() == colors.size());
    glGenVertexArrays(1, &_id_array);
    generateBuffer(positions, radius, colors);
    _size = radius.size() * 2;
  }

  ~CylinderSide() {
    glDeleteBuffers(1, (GLuint *)&_buffer_position);
    glDeleteBuffers(1, (GLuint *)&_buffer_color);
    glDeleteBuffers(1, (GLuint *)&_buffer_radius);
    glDeleteVertexArrays(1, &_id_array);
  }

  void display(const glm::mat4 &proj) {
    GLuint id_transform = glGetUniformLocation(_id_program, "transform");
    GLuint id_proj = glGetUniformLocation(_id_program, "proj");
    glUseProgram(_id_program);

    //      glm::mat4 total_proj = proj * _transform;
    glUniformMatrix4fv(id_proj, 1, GL_FALSE, &proj[0][0]);
    glUniformMatrix4fv(id_transform, 1, GL_FALSE, &_transform[0][0]);
    glBindVertexArray(_id_array);

    glEnableVertexAttribArray(0);
    glBindBuffer(GL_ARRAY_BUFFER, _buffer_position);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, NULL);

    glEnableVertexAttribArray(1);
    glBindBuffer(GL_ARRAY_BUFFER, _buffer_color);
    glVertexAttribPointer(1, 4, GL_FLOAT, GL_FALSE, 0, NULL);

    glEnableVertexAttribArray(2);
    glBindBuffer(GL_ARRAY_BUFFER, _buffer_radius);
    glVertexAttribPointer(2, 1, GL_FLOAT, GL_FALSE, 0, NULL);

    glDrawArrays(GL_LINES, 0, _size);

    glDisableVertexAttribArray(2);
    glDisableVertexAttribArray(1);
    glDisableVertexAttribArray(0);
  }
};
}  // namespace loco

#endif  // COMMONVISUALIZER_CYLINDERSIDE_HPP

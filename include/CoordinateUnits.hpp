//
// Created by tlou on 30.01.17.
//

#ifndef COMMONVISUALIZER_COORDINATEUNITS_HPP
#define COMMONVISUALIZER_COORDINATEUNITS_HPP

#include "Object.hpp"

namespace loco {
class CoordinateUnits : public Object {
 protected:
  GLuint _buffer_color;

 public:
  CoordinateUnits(const GLuint id_program,
                  const Transform &transform =
                      Transform{Vec{}, Vec{0.0f, 0.0f, 0.0f, 1.0f}},
                  const float scale = 1.0f)
      : Object(id_program) {
    std::vector<float> points;
    std::vector<float> colors;
    points.reserve(6 * 3);
    colors.reserve(6 * 4);
    // add points
    float start[] = {0.0f, 0.0f, 0.0f, 1.0f};
    for (int id_dim = 0; id_dim < 3; ++id_dim) {
      float color[] = {0.0f, 0.0f, 0.0f, 1.0f};
      float end[] = {0.0f, 0.0f, 0.0f, 1.0f};
      end[id_dim] = scale;
      color[id_dim] = 1.0f;
      for (int i = 0; i < 3; ++i) {
        points.push_back(start[i]);
      }
      for (int i = 0; i < 3; ++i) {
        points.push_back(end[i]);
      }
      for (int i = 0; i < 8; ++i) {
        colors.push_back(color[i % 4]);
      }
    }
    glGenVertexArrays(1, &_id_array);
    genBufferVectorFloat(points, _buffer_position);
    genBufferVectorFloat(colors, _buffer_color);
    _size = 6;
  }

  ~CoordinateUnits() {
    glDeleteBuffers(1, (GLuint *)&_buffer_color);
    glDeleteBuffers(1, (GLuint *)&_buffer_position);
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

    glDrawArrays(GL_LINES, 0, _size);

    glDisableVertexAttribArray(1);
    glDisableVertexAttribArray(0);
  }
};
}  // namespace loco

#endif  // COMMONVISUALIZER_COORDINATEUNITS_HPP

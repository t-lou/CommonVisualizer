//
// Created by tlou on 30.01.17.
//

#ifndef COMMONVISUALIZER_CLOUDSPHEREUNICOLOR_HPP
#define COMMONVISUALIZER_CLOUDSPHEREUNICOLOR_HPP

#include "Object.hpp"

namespace loco {
class CloudSphereUnicolor : public Object {
 protected:
  const float _radius;
  const Vec _color;
  GLint _id_transform = -1;
  GLint _id_proj = -1;
  GLint _id_size = -1;
  GLint _id_color = -1;

 public:
  CloudSphereUnicolor(const std::vector<float> &positions, const Vec &color,
                      const float radius, const GLuint id_program)
      : Object(id_program), _radius(radius), _color(color) {
    assert(positions.size() % 3u == 0u);
    // vertex array
    glGenVertexArrays(1, &_id_array);
    genBufferVectorFloat(positions, _buffer_position);
    _size = positions.size() / 3u;

    _id_transform = glGetUniformLocation(_id_program, "transform");
    _id_proj = glGetUniformLocation(_id_program, "proj");
    _id_size = glGetUniformLocation(_id_program, "radius");
    _id_color = glGetUniformLocation(_id_program, "color");

    glBindVertexArray(_id_array);
    glBindBuffer(GL_ARRAY_BUFFER, _buffer_position);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, NULL);
    glBindVertexArray(0);
  }

  ~CloudSphereUnicolor() {
    glDeleteBuffers(1, (GLuint *)&_buffer_position);
    glDeleteVertexArrays(1, &_id_array);
  }

  void display(const glm::mat4 &proj) {
    glUseProgram(_id_program);

    glUniformMatrix4fv(_id_proj, 1, GL_FALSE, &proj[0][0]);
    glUniformMatrix4fv(_id_transform, 1, GL_FALSE, &_transform[0][0]);

    glUniform1f(_id_size, _radius);
    glUniform4fv(_id_color, 1, _color.data);
    glBindVertexArray(_id_array);

    glDrawArrays(GL_POINTS, 0, _size);
  }

  void updatePointPositions(const std::vector<float> &positions) override {
    updatePositionBuffer(positions);
  }
};
}  // namespace loco

#endif  // COMMONVISUALIZER_CLOUDSPHEREUNICOLOR_HPP

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

 public:
  CloudSphereUnicolor(const std::vector<float> &positions, const Vec &color,
                      const float radius, const GLuint id_program)
      : Object(id_program), _radius(radius), _color(color) {
    assert(positions.size() % 3u == 0u);
    // vertex array
    glGenVertexArrays(1, &_id_array);
    genBufferVectorFloat(positions, _buffer_position);
    _size = positions.size() / 3u;
  }

  ~CloudSphereUnicolor() {
    glDeleteBuffers(1, (GLuint *)&_buffer_position);
    glDeleteVertexArrays(1, &_id_array);
  }

  void display(const glm::mat4 &proj) {
    GLuint id_transform = glGetUniformLocation(_id_program, "transform");
    GLuint id_proj = glGetUniformLocation(_id_program, "proj");
    GLuint id_size = glGetUniformLocation(_id_program, "radius");
    GLuint id_color = glGetUniformLocation(_id_program, "color");
    glUseProgram(_id_program);

    glUniformMatrix4fv(id_proj, 1, GL_FALSE, &proj[0][0]);
    glUniformMatrix4fv(id_transform, 1, GL_FALSE, &_transform[0][0]);

    glUniform1f(id_size, _radius);
    glUniform4fv(id_color, 1, _color.data);
    glBindVertexArray(_id_array);

    glEnableVertexAttribArray(0);
    glBindBuffer(GL_ARRAY_BUFFER, _buffer_position);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, NULL);

    glDrawArrays(GL_POINTS, 0, _size);

    glDisableVertexAttribArray(0);
  }
};
}  // namespace loco

#endif  // COMMONVISUALIZER_CLOUDSPHEREUNICOLOR_HPP

//
// Created by tlou on 30.01.17.
//

#ifndef COMMONVISUALIZER_CLOUDPOINTCOLORED_HPP
#define COMMONVISUALIZER_CLOUDPOINTCOLORED_HPP

#include "Object.hpp"

namespace loco {
class CloudPointColored : public Object {
 protected:
  const float _point_size;
  GLuint _buffer_color;

 public:
  /**
   * constructor of array of colored points
   * @param positions
   * @param colors
   * @param point_size size of points
   * @param id_program
   */
  CloudPointColored(const std::vector<float> &positions,
                    const std::vector<float> &colors, const float point_size,
                    const GLuint id_program)
      : Object(id_program), _point_size(std::max(1.0f, point_size)) {
    assert(positions.size() * 4u == colors.size() * 3u);
    // vertex array
    glGenVertexArrays(1, &_id_array);
    genBufferVectorFloat(positions, _buffer_position);
    genBufferVectorFloat(colors, _buffer_color);
    _size = positions.size() / 3u;
  }

  ~CloudPointColored() {
    glDeleteBuffers(1, (GLuint *)&_buffer_color);
    glDeleteBuffers(1, (GLuint *)&_buffer_position);
    glDeleteVertexArrays(1, &_id_array);
  }

  void display(const glm::mat4 &proj) {
    GLuint id_transform = glGetUniformLocation(_id_program, "transform");
    GLuint id_proj = glGetUniformLocation(_id_program, "proj");
    GLuint id_size = glGetUniformLocation(_id_program, "point_size");
    glUseProgram(_id_program);

    glUniformMatrix4fv(id_proj, 1, GL_FALSE, &proj[0][0]);
    glUniformMatrix4fv(id_transform, 1, GL_FALSE, &_transform[0][0]);
    glUniform1f(id_size, _point_size);
    glBindVertexArray(_id_array);

    glEnableVertexAttribArray(0);
    glBindBuffer(GL_ARRAY_BUFFER, _buffer_position);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, NULL);

    glEnableVertexAttribArray(1);
    glBindBuffer(GL_ARRAY_BUFFER, _buffer_color);
    glVertexAttribPointer(1, 4, GL_FLOAT, GL_FALSE, 0, NULL);

    glDrawArrays(GL_POINTS, 0, _size);

    glDisableVertexAttribArray(1);
    glDisableVertexAttribArray(0);
  }
};
}  // namespace loco

#endif  // COMMONVISUALIZER_CLOUDPOINTCOLORED_HPP

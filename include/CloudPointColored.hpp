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
  GLint _id_transform = -1;
  GLint _id_proj = -1;
  GLint _id_size = -1;

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

    _id_transform = glGetUniformLocation(_id_program, "transform");
    _id_proj = glGetUniformLocation(_id_program, "proj");
    _id_size = glGetUniformLocation(_id_program, "point_size");

    glBindVertexArray(_id_array);
    glBindBuffer(GL_ARRAY_BUFFER, _buffer_position);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, NULL);
    glBindBuffer(GL_ARRAY_BUFFER, _buffer_color);
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 4, GL_FLOAT, GL_FALSE, 0, NULL);
    glBindVertexArray(0);
  }

  ~CloudPointColored() {
    glDeleteBuffers(1, (GLuint *)&_buffer_color);
    glDeleteBuffers(1, (GLuint *)&_buffer_position);
    glDeleteVertexArrays(1, &_id_array);
  }

  void display(const glm::mat4 &proj) {
    glUseProgram(_id_program);

    glUniformMatrix4fv(_id_proj, 1, GL_FALSE, &proj[0][0]);
    glUniformMatrix4fv(_id_transform, 1, GL_FALSE, &_transform[0][0]);
    glUniform1f(_id_size, _point_size);
    glBindVertexArray(_id_array);

    glDrawArrays(GL_POINTS, 0, _size);
  }

  void updatePointPositions(const std::vector<float> &positions) override {
    updatePositionBuffer(positions);
  }
};
}  // namespace loco

#endif  // COMMONVISUALIZER_CLOUDPOINTCOLORED_HPP

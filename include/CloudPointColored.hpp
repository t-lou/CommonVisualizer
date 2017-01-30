//
// Created by tlou on 30.01.17.
//

#ifndef COMMONVISUALIZER_CLOUDPOINTCOLORED_HPP
#define COMMONVISUALIZER_CLOUDPOINTCOLORED_HPP

#include "Object.hpp"

namespace loco
{
  class CloudPointColored : public Object
  {
  protected:
    float _point_size;
    GLuint _buffer_color;

  public:
    CloudPointColored(const std::vector<float> &positions, const std::vector<float> &colors,
                      const float point_size, const GLuint id_program) :
        Object(id_program),
        _point_size(point_size > 1.0f ? point_size : 1.0f)
    {
      assert(positions.size() * 4 == colors.size() * 3);
      // vertex array
      glGenVertexArrays(1, &_id_array);
      genBufferVectorFloat(positions, _buffer_position);
      genBufferVectorFloat(colors, _buffer_color);
      _size = positions.size() / 3;
    }

    ~CloudPointColored()
    {
      glDeleteBuffers(1, (GLuint *) &_buffer_color);
      glDeleteBuffers(1, (GLuint *) &_buffer_position);
      glDeleteVertexArrays(1, &_id_array);
    }

    void display(const glm::mat4 &proj)
    {
      GLuint id_proj = glGetUniformLocation(_id_program, "proj");
      GLuint id_size = glGetUniformLocation(_id_program, "point_size");
      glUseProgram(_id_program);

      glm::mat4 total_proj = proj * _transform;
      glUniformMatrix4fv(id_proj, 1, GL_FALSE, &total_proj[0][0]);
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
}

#endif //COMMONVISUALIZER_CLOUDPOINTCOLORED_HPP

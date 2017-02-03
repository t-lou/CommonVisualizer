//
// Created by tlou on 30.01.17.
//

#ifndef COMMONVISUALIZER_CLOUDSPHERECOLORED_HPP
#define COMMONVISUALIZER_CLOUDSPHERECOLORED_HPP

#include "Object.hpp"

namespace loco
{
  class CloudSphereColored : public Object
  {
  protected:
    float _radius;
    GLuint _buffer_color;

  public:
    CloudSphereColored(const std::vector<float> &positions, const std::vector<float> &colors,
                       const float radius, const GLuint id_program) :
        Object(id_program),
        _radius(radius)
    {
      assert(positions.size() * 4 == colors.size() * 3);
      // vertex array
      glGenVertexArrays(1, &_id_array);
      genBufferVectorFloat(positions, _buffer_position);
      genBufferVectorFloat(colors, _buffer_color);
      _size = positions.size() / 3;
    }

    ~CloudSphereColored()
    {
      glDeleteBuffers(1, (GLuint *) &_buffer_color);
      glDeleteBuffers(1, (GLuint *) &_buffer_position);
      glDeleteVertexArrays(1, &_id_array);
    }

    void display(const glm::mat4 &proj)
    {
      GLuint id_transform = glGetUniformLocation(_id_program, "transform");
      GLuint id_proj = glGetUniformLocation(_id_program, "proj");
      GLuint id_size = glGetUniformLocation(_id_program, "radius");
      glUseProgram(_id_program);

//      glm::mat4 total_proj = proj * _transform;
      glUniformMatrix4fv(id_proj, 1, GL_FALSE, &proj[0][0]);
      glUniformMatrix4fv(id_transform, 1, GL_FALSE, &_transform[0][0]);

      glUniform1f(id_size, _radius);
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

#endif //COMMONVISUALIZER_CLOUDSPHERECOLORED_HPP

//
// Created by tlou on 01.02.17.
//

#ifndef COMMONVISUALIZER_ORIENTEDCIRCLE_HPP
#define COMMONVISUALIZER_ORIENTEDCIRCLE_HPP

#include "Object.hpp"

namespace loco
{
  class OrientedCircle : public Object
  {
  protected:
    GLuint _buffer_color;
    GLuint _buffer_normal;
    GLuint _buffer_radius;

  public:
    OrientedCircle(const std::vector<float> &positions, const std::vector<float> &normals,
                   const std::vector<float> &radius, const std::vector<Vec> &colors,
                   const GLuint id_program) :
        Object(id_program)
    {
      assert(radius.size() == colors.size());
      assert(positions.size() == colors.size() * 3u);
      assert(normals.size() == colors.size() * 3u);

      std::vector<float> colors_;
      colors_.reserve(radius.size() * 4u);
      for(const Vec &color : colors)
      {
        colors_.insert(colors_.end(), color._data, color._data + 4);
      }

      glGenVertexArrays(1, &_id_array);
      genBufferVectorFloat(positions, _buffer_position);
      genBufferVectorFloat(normals, _buffer_normal);
      genBufferVectorFloat(colors_, _buffer_color);
      genBufferVectorFloat(radius, _buffer_radius);

      _size = colors.size();
    }

    ~OrientedCircle()
    {
      glDeleteBuffers(1, (GLuint *) &_buffer_normal);
      glDeleteBuffers(1, (GLuint *) &_buffer_position);
      glDeleteBuffers(1, (GLuint *) &_buffer_color);
      glDeleteBuffers(1, (GLuint *) &_buffer_radius);
      glDeleteVertexArrays(1, &_id_array);
    }

    void display(const glm::mat4 &proj)
    {
      GLuint id_transform = glGetUniformLocation(_id_program, "transform");
      GLuint id_proj = glGetUniformLocation(_id_program, "proj");
      GLuint id_color = glGetUniformLocation(_id_program, "color");
      glUseProgram(_id_program);

//      glm::mat4 total_proj = proj * _transform;
      glUniformMatrix4fv(id_proj, 1, GL_FALSE, &proj[0][0]);
      glUniformMatrix4fv(id_transform, 1, GL_FALSE, &_transform[0][0]);
      glBindVertexArray(_id_array);

      glEnableVertexAttribArray(0);
      glBindBuffer(GL_ARRAY_BUFFER, _buffer_position);
      glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, NULL);

      glEnableVertexAttribArray(1);
      glBindBuffer(GL_ARRAY_BUFFER, _buffer_normal);
      glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 0, NULL);

      glEnableVertexAttribArray(2);
      glBindBuffer(GL_ARRAY_BUFFER, _buffer_color);
      glVertexAttribPointer(2, 4, GL_FLOAT, GL_FALSE, 0, NULL);

      glEnableVertexAttribArray(3);
      glBindBuffer(GL_ARRAY_BUFFER, _buffer_radius);
      glVertexAttribPointer(3, 1, GL_FLOAT, GL_FALSE, 0, NULL);

      glDrawArrays(GL_POINTS, 0, _size);

      glDisableVertexAttribArray(3);
      glDisableVertexAttribArray(2);
      glDisableVertexAttribArray(1);
      glDisableVertexAttribArray(0);
    }
  };
}

#endif //COMMONVISUALIZER_ORIENTEDCIRCLE_HPP

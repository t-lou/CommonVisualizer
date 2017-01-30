//
// Created by tlou on 30.01.17.
//

#ifndef COMMONVISUALIZER_CYLINDERSIDE_HPP
#define COMMONVISUALIZER_CYLINDERSIDE_HPP

#include "Object.hpp"

namespace loco
{
  class CylinderSide : public Object
  {
  protected:
    Vec _color;
    GLuint _buffer_radius;
    GLuint _buffer_color;

    void initBuffer(const std::vector<float>& positions,
                    const std::vector<float> radius,
                    const std::vector<Vec> &colors)
    {
      assert(positions.size() % 6 == 0);
      assert(positions.size() / 6 == radius.size());
      assert(radius.size() == colors.size());

      // unfold data
      std::vector<float> radius_;
      std::vector<float> colors_;
      std::vector<float> heights_;
      radius_.reserve(radius.size() * 2);
      colors_.reserve(colors.size() * 8);
      for(size_t id = 0; id < radius.size(); ++id)
      {
        for(int i = 0; i < 2; ++i)
        {
          radius_.push_back(radius.at(id));
          colors_.push_back(colors.at(id)._r);
          colors_.push_back(colors.at(id)._g);
          colors_.push_back(colors.at(id)._b);
          colors_.push_back(colors.at(id)._a);
        }
      }

      // prepare buffer
      genBufferVectorFloat(positions, _buffer_position);
      genBufferVectorFloat(radius_, _buffer_radius);
      genBufferVectorFloat(colors_, _buffer_color);
    }

  public:
    CylinderSide(const std::vector<float> &positions,
                 const std::vector<float> radius, const std::vector<Vec> &colors,
                 const GLuint &id_program) :
        Object(id_program)
    {
      initBuffer(positions, radius, colors);
      glGenVertexArrays(1, &_id_array);
      _size = radius.size();
    }

    CylinderSide(const std::vector<Transform> &transforms, const std::vector<float> &heights,
                 const std::vector<float> &radius, const std::vector<Vec>& color,
                 const GLuint &id_program) :
        Object(id_program)
    {
      // TODO
      glGenVertexArrays(1, &_id_array);
      _size = radius.size();
    }

    ~CylinderSide()
    {
      glDeleteBuffers(1, (GLuint *) &_buffer_position);
      glDeleteBuffers(1, (GLuint *) &_buffer_radius);
      glDeleteBuffers(1, (GLuint *) &_buffer_color);
      glDeleteVertexArrays(1, &_id_array);
    }

    void display(const glm::mat4 &proj)
    {
      GLuint id_proj = glGetUniformLocation(_id_program, "proj");
      glUseProgram(_id_program);

      glm::mat4 total_proj = proj * _transform;
      glUniformMatrix4fv(id_proj, 1, GL_FALSE, &total_proj[0][0]);
      glBindVertexArray(_id_array);

      glEnableVertexAttribArray(0);
      glBindBuffer(GL_ARRAY_BUFFER, _buffer_position);
      glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, NULL);

      glEnableVertexAttribArray(1);
      glBindBuffer(GL_ARRAY_BUFFER, _buffer_radius);
      glVertexAttribPointer(1, 1, GL_FLOAT, GL_FALSE, 0, NULL);

      glEnableVertexAttribArray(2);
      glBindBuffer(GL_ARRAY_BUFFER, _buffer_color);
      glVertexAttribPointer(2, 4, GL_FLOAT, GL_FALSE, 0, NULL);

      glDrawArrays(GL_LINES, 0, _size);

      glDisableVertexAttribArray(2);
      glDisableVertexAttribArray(1);
      glDisableVertexAttribArray(0);
    }
  };
}

#endif //COMMONVISUALIZER_CYLINDERSIDE_HPP

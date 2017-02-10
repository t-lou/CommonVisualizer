//
// Created by tlou on 29.01.17.
//

#ifndef COMMONVISUALIZER_MESHCOLORED_HPP
#define COMMONVISUALIZER_MESHCOLORED_HPP

#include "Object.hpp"

namespace loco
{
  class MeshColored : public Object
  {
  protected:
    GLuint _buffer_normal;
    GLuint _buffer_color;

  public:
    MeshColored(const std::vector<float> &vertices, const std::vector<float> &normals,
                const std::vector<float> &colors, const GLuint id_program):
        Object(id_program)
    {
      assert(vertices.size() * 12 == colors.size() * 9);
      assert(vertices.size() == normals.size());
      // vertex array
      glGenVertexArrays(1, &_id_array);
      // vertex buffer
      genBufferVectorFloat(vertices, _buffer_position);
      // normal buffer
      genBufferVectorFloat(normals, _buffer_normal);
      // color buffer
      genBufferVectorFloat(colors, _buffer_color);
      _size = vertices.size() / 3;
    }

    MeshColored(const std::vector<float> &vertices,
                const std::vector<float> &colors, const GLuint id_program) :
        MeshColored(vertices, genFakeNormal(vertices), colors, id_program)
    {
    }

    MeshColored(const std::vector<float> &vertices, const std::vector<float> &colors,
                const std::vector<int> &index,const GLuint id_program) :
        MeshColored(unfoldList(vertices, index, 3), genFakeNormal(unfoldList(vertices, index, 3)),
                    unfoldList(colors, index, 4), id_program)
    {
    }

    MeshColored(const std::vector<float> &vertices, const std::vector<float> &normals,
                const std::vector<float> &colors, const std::vector<int> &index,const GLuint id_program) :
        MeshColored(unfoldList(vertices, index, 3), unfoldList(normals, index, 3),
                    unfoldList(colors, index, 4), id_program)
    {
    }

    ~MeshColored()
    {
      glDeleteBuffers(1, (GLuint *) &_buffer_color);
      glDeleteBuffers(1, (GLuint *) &_buffer_normal);
      glDeleteBuffers(1, (GLuint *) &_buffer_position);
      glDeleteVertexArrays(1, &_id_array);
    }

    void display(const glm::mat4 &proj)
    {
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
      glBindBuffer(GL_ARRAY_BUFFER, _buffer_normal);
      glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, 0, NULL);

      glDrawArrays(GL_TRIANGLES, 0, _size);

      glDisableVertexAttribArray(2);
      glDisableVertexAttribArray(1);
      glDisableVertexAttribArray(0);
    }
  };
}

#endif //COMMONVISUALIZER_MESHCOLORED_HPP

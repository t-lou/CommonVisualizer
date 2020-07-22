//
// Created by tlou on 29.01.17.
//

#ifndef COMMONVISUALIZER_MESHUNICOLOR_HPP
#define COMMONVISUALIZER_MESHUNICOLOR_HPP

#include "Object.hpp"

namespace loco {
class MeshUnicolor : public Object {
 protected:
  GLuint _buffer_normal;
  const Vec _color;

 public:
  MeshUnicolor(const std::vector<float> &vertices,
               const std::vector<float> &normals, const Vec &color,
               const GLuint id_program)
      : Object(id_program), _color(color) {
    assert(vertices.size() % 9 == 0);
    assert(normals.size() == vertices.size());
    // vertex array
    glGenVertexArrays(1, &_id_array);
    // vertex buffer
    genBufferVectorFloat(vertices, _buffer_position);
    // normal buffer
    genBufferVectorFloat(normals, _buffer_normal);
    _size = vertices.size() / 3;
  }

  MeshUnicolor(const std::vector<float> &vertices, const Vec &color,
               const GLuint id_program)
      : MeshUnicolor(vertices, genFakeNormal(vertices), color, id_program) {}

  MeshUnicolor(const std::vector<float> &vertices,
               const std::vector<float> &normals, const std::vector<int> &index,
               const Vec &color, const GLuint id_program)
      : MeshUnicolor(unfoldList(vertices, index, 3),
                     unfoldList(normals, index, 3), color, id_program) {}

  MeshUnicolor(const std::vector<float> &vertices,
               const std::vector<int> &index, const Vec &color,
               const GLuint id_program)
      : MeshUnicolor(unfoldList(vertices, index, 3), color, id_program) {}

  ~MeshUnicolor() {
    glDeleteBuffers(1, (GLuint *)&_buffer_normal);
    glDeleteBuffers(1, (GLuint *)&_buffer_position);
    glDeleteVertexArrays(1, &_id_array);
  }

  void display(const glm::mat4 &proj) {
    GLuint id_transform = glGetUniformLocation(_id_program, "transform");
    GLuint id_proj = glGetUniformLocation(_id_program, "proj");
    GLuint id_color = glGetUniformLocation(_id_program, "color");
    glUseProgram(_id_program);

    glUniformMatrix4fv(id_proj, 1, GL_FALSE, &proj[0][0]);
    glUniformMatrix4fv(id_transform, 1, GL_FALSE, &_transform[0][0]);
    glUniform4fv(id_color, 1, _color._data);
    glBindVertexArray(_id_array);

    glEnableVertexAttribArray(0);
    glBindBuffer(GL_ARRAY_BUFFER, _buffer_position);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, NULL);

    glEnableVertexAttribArray(1);
    glBindBuffer(GL_ARRAY_BUFFER, _buffer_normal);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 0, NULL);

    glDrawArrays(GL_TRIANGLES, 0, _size);

    glDisableVertexAttribArray(1);
    glDisableVertexAttribArray(0);
  }
};
}  // namespace loco

#endif  // COMMONVISUALIZER_MESHUNICOLOR_HPP

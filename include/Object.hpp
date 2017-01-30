//
// Created by tlou on 29.01.17.
//

#ifndef COMMONVISUALIZER_OBJECT_H
#define COMMONVISUALIZER_OBJECT_H

#include <vector>
#include <memory>
#include "include_gl.h"
#include "Vec.h"

namespace loco
{
  class Object
  {
  protected:
    glm::mat4 _transform;
    GLuint _id_program;
    GLuint _id_array;
    GLuint _buffer_position;
    int _size;

    static void genBufferVectorFloat(const std::vector<float>& data, GLuint &id_buffer)
    {
      glGenBuffers(1, &id_buffer);
      glBindBuffer(GL_ARRAY_BUFFER, id_buffer);
      glBufferData(GL_ARRAY_BUFFER, 4 * data.size(), &data.front(), GL_STATIC_DRAW);
    }

    static std::vector<float> genFakeNormal(const std::vector<float> &vertices)
    {
      std::vector<float> normals;
      normals.resize(vertices.size());
      for(size_t id = 0; id < vertices.size(); id += 9)
      {
        // use the cross product of two edges for normal
        // vec0 <- vector from second to first
        glm::vec3 vec0(vertices.at(id + 3) - vertices.at(id + 0),
                       vertices.at(id + 4) - vertices.at(id + 1),
                       vertices.at(id + 5) - vertices.at(id + 2));
        // vec1 <- vector from second to third
        glm::vec3 vec1(vertices.at(id + 3) - vertices.at(id + 6),
                       vertices.at(id + 4) - vertices.at(id + 7),
                       vertices.at(id + 5) - vertices.at(id + 8));
        glm::vec3 nor = glm::normalize(-glm::cross(vec0, vec1));
        for(int i = 0; i < 3; ++i)
        {
          normals.at(id + i * 3 + 0) = nor[0];
          normals.at(id + i * 3 + 1) = nor[1];
          normals.at(id + i * 3 + 2) = nor[2];
        }
      }
      return normals;
    }

  public:
    Object(const GLuint id_program):
        _id_program(id_program),
        _transform(glm::mat4(1.0f))
    {
    }

    ~Object()
    {
    }

    static glm::mat4 transformToMat4(const Transform &transform)
    {
      glm::vec4 quat(transform._rotation._x, transform._rotation._y,
                     transform._rotation._z, transform._rotation._w);
      quat /= glm::length(quat);
      glm::mat4 tf = glm::mat4_cast(glm::quat(quat.w, quat.x, quat.y, quat.z));
      tf[3] = glm::vec4(transform._translation._x, transform._translation._y, transform._translation._z, 1.0f);
      return tf;
    }

    void setTransform(const Transform &transform)
    {
      _transform = Object::transformToMat4(transform);
    }

    virtual void display(const glm::mat4 &proj) = 0;

  };
}

#endif //COMMONVISUALIZER_OBJECT_H

//
// Created by tlou on 29.01.17.
//

#ifndef COMMONVISUALIZER_OBJECT_H
#define COMMONVISUALIZER_OBJECT_H

#include <GL/glew.h>
#include <GL/glut.h>
#include <GLFW/glfw3.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/quaternion.hpp>
#include <memory>
#include <vector>

#include "DataStructure.h"

namespace loco {
class Object {
 protected:
  GLuint _id_program;
  glm::mat4 _transform;
  GLuint _id_array;
  GLuint _buffer_position;
  int _size;

  /**
   * generate buffer for vector of float
   * @param data
   * @param id_buffer
   */
  static void genBufferVectorFloat(const std::vector<float> &data,
                                   GLuint &id_buffer) {
    glGenBuffers(1, &id_buffer);
    glBindBuffer(GL_ARRAY_BUFFER, id_buffer);
    glBufferData(GL_ARRAY_BUFFER, 4 * data.size(), &data.front(),
                 GL_STATIC_DRAW);
  }

  /**
   * compute normal vectors for triangles with vertices, the computed normals
   * are perpendicular to triangles
   * @param vertices (x, y, z) positions of triangles
   * @return
   */
  static std::vector<float> genFakeNormal(const std::vector<float> &vertices) {
    assert(vertices.size() % 3 == 0);
    std::vector<float> normals;
    normals.resize(vertices.size());
    for (size_t id = 0; id < vertices.size(); id += 9) {
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
      for (int i = 0; i < 3; ++i) {
        normals.at(id + i * 3 + 0) = nor[0];
        normals.at(id + i * 3 + 1) = nor[1];
        normals.at(id + i * 3 + 2) = nor[2];
      }
    }
    return normals;
  }

  /**
   * unfold vector according to index.
   * @param data
   * @param index
   * @param length_vector length of unity data. usually 3 for position and 4 for
   * color. each indice in index will be replaced with
   *        data[indice/length_vector][0] ~
   * data[indice/length_vector][length_vector]
   * @return
   */
  static std::vector<float> unfoldList(const std::vector<float> &data,
                                       const std::vector<int> &index,
                                       const int length_vector) {
    std::vector<float> unfolded;
    unfolded.reserve(index.size() * length_vector);

    for (int id : index) {
      assert(length_vector * (id + 1) <= data.size());
      unfolded.insert(unfolded.end(), data.begin() + length_vector * id,
                      data.begin() + length_vector * (id + 1));
    }
    return unfolded;
  }

 public:
  Object() : Object(0) {}

  Object(const GLuint id_program)
      : _id_program(id_program), _transform(glm::mat4(1.0f)) {}

  virtual ~Object() {}

  /**
   * convert transform with format Transform to glm::mat4
   * @param transform
   * @return
   */
  static glm::mat4 transformToMat4(const Transform &transform) {
    glm::vec4 quat(transform.rotation.xyzw.x, transform.rotation.xyzw.y,
                   transform.rotation.xyzw.z, transform.rotation.xyzw.w);
    quat /= glm::length(quat);
    glm::mat4 tf = glm::mat4_cast(glm::quat(quat.w, quat.x, quat.y, quat.z));
    tf[3] =
        glm::vec4(transform.translation.xyzw.x, transform.translation.xyzw.y,
                  transform.translation.xyzw.z, 1.0f);
    return tf;
  }

  /**
   * set local transform
   * @param transform
   */
  virtual void setTransform(const Transform &transform) {
    _transform = Object::transformToMat4(transform);
  }

  /**
   * apply transform to local transform
   * @param transform
   */
  virtual void applyTransform(const Transform &transform) {
    _transform *= Object::transformToMat4(transform);
  }

  /**
   * display function. to override for subclass
   * @param proj
   */
  virtual void display(const glm::mat4 &proj) = 0;
};
}  // namespace loco

#endif  // COMMONVISUALIZER_OBJECT_H

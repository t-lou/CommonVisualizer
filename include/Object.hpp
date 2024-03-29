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
  std::size_t _size;

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
    assert(vertices.size() % 3u == 0u);
    std::vector<float> normals;
    normals.resize(vertices.size());
    for (std::size_t id = 0u; id < vertices.size(); id += 9u) {
      // use the cross product of two edges for normal
      // vec0 <- vector from second to first
      glm::vec3 vec0(vertices.at(id + 3u) - vertices.at(id + 0u),
                     vertices.at(id + 4u) - vertices.at(id + 1u),
                     vertices.at(id + 5u) - vertices.at(id + 2u));
      // vec1 <- vector from second to third
      glm::vec3 vec1(vertices.at(id + 3u) - vertices.at(id + 6u),
                     vertices.at(id + 4u) - vertices.at(id + 7u),
                     vertices.at(id + 5u) - vertices.at(id + 8u));
      glm::vec3 nor = glm::normalize(-glm::cross(vec0, vec1));
      for (std::size_t i = 0u; i < 3u; ++i) {
        normals.at(id + i * 3u     ) = nor[0u];
        normals.at(id + i * 3u + 1u) = nor[1u];
        normals.at(id + i * 3u + 2u) = nor[2u];
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
                                       const std::vector<std::uint32_t> &index,
                                       const std::size_t length_vector) {
    std::vector<float> unfolded;
    unfolded.reserve(index.size() * length_vector);

    for (std::uint32_t id : index) {
      assert(length_vector * (id + 1u) <= data.size());
      unfolded.insert(unfolded.end(), data.begin() + length_vector * id,
                      data.begin() + length_vector * (id + 1));
    }
    return unfolded;
  }

 public:
  Object() : Object(0u) {}

  Object(const GLuint id_program)
      : _id_program(id_program), _transform(glm::mat4(1.0f)) {}

  virtual ~Object() {}

  /**
   * convert transform with format Transform to glm::mat4
   * @param transform
   * @return
   */
  static glm::mat4 transformToMat4(const Transform &transform) {
    glm::vec4 quat(transform.rotation.to_glm_vec4());
    quat /= glm::length(quat);
    glm::mat4 tf = glm::mat4_cast(glm::quat(quat.w, quat.x, quat.y, quat.z));
    tf[3] = glm::vec4(transform.translation.to_glm_vec3(), 1.0f);
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

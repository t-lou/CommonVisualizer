//
// Created by tlou on 30.01.17.
//

#ifndef COMMONVISUALIZER_BOX_HPP
#define COMMONVISUALIZER_BOX_HPP

#include "MeshUnicolor.hpp"

namespace loco
{
  class Box : public MeshUnicolor
  {
  protected:
    /**
     * create a vector of point positions of corners for drawing. it is shit but it works
     * @param transform
     * @param scale
     * @return
     */
    std::vector<float> getVertex(const Transform &transform, const Vec &scale)
    {
      glm::mat4 tf_mat = Object::transformToMat4(transform);
      std::vector<float> corners;
      std::vector<float> vertex;
      corners.reserve(24);
      vertex.reserve(108);

      // get transformed corners
      for(int id = 0; id < 8; ++id)
      {
        glm::vec4 corner(scale._x, scale._y, scale._z, 1.0f);
        if(id >= 4)
        {
          corner.z = -corner.z;
        }
        if(id & 2)
        {
          corner.x = -corner.x;
        }
        if(id % 2 == 0)
        {
          corner.y = -corner.y;
        }

        corner = tf_mat * corner;

        for(int i = 0; i < 3; ++i)
        {
          corners.push_back(corner[i]);
        }
      }

      // triangulate mesh
      std::vector<float>::iterator begin = corners.begin();

      vertex.insert(vertex.end(), begin, begin + 3);
      vertex.insert(vertex.end(), begin + 3, begin + 6);
      vertex.insert(vertex.end(), begin + 6, begin + 9);
      vertex.insert(vertex.end(), begin + 6, begin + 9);
      vertex.insert(vertex.end(), begin + 3, begin + 6);
      vertex.insert(vertex.end(), begin + 9, begin + 12);

      vertex.insert(vertex.end(), begin + 12, begin + 15);
      vertex.insert(vertex.end(), begin + 18, begin + 21);
      vertex.insert(vertex.end(), begin + 15, begin + 18);
      vertex.insert(vertex.end(), begin + 15, begin + 18);
      vertex.insert(vertex.end(), begin + 18, begin + 21);
      vertex.insert(vertex.end(), begin + 21, begin + 24);

      vertex.insert(vertex.end(), begin, begin + 3);
      vertex.insert(vertex.end(), begin + 15, begin + 18);
      vertex.insert(vertex.end(), begin + 3, begin + 6);
      vertex.insert(vertex.end(), begin + 12, begin + 15);
      vertex.insert(vertex.end(), begin + 15, begin + 18);
      vertex.insert(vertex.end(), begin, begin + 3);

      vertex.insert(vertex.end(), begin + 6, begin + 9);
      vertex.insert(vertex.end(), begin + 9, begin + 12);
      vertex.insert(vertex.end(), begin + 18, begin + 21);
      vertex.insert(vertex.end(), begin + 21, begin + 24);
      vertex.insert(vertex.end(), begin + 18, begin + 21);
      vertex.insert(vertex.end(), begin + 9, begin + 12);

      vertex.insert(vertex.end(), begin, begin + 3);
      vertex.insert(vertex.end(), begin + 6, begin + 9);
      vertex.insert(vertex.end(), begin + 18, begin + 21);
      vertex.insert(vertex.end(), begin + 18, begin + 21);
      vertex.insert(vertex.end(), begin + 12, begin + 15);
      vertex.insert(vertex.end(), begin, begin + 3);

      vertex.insert(vertex.end(), begin + 9, begin + 12);
      vertex.insert(vertex.end(), begin + 3, begin + 6);
      vertex.insert(vertex.end(), begin + 21, begin + 24);
      vertex.insert(vertex.end(), begin + 21, begin + 24);
      vertex.insert(vertex.end(), begin + 3, begin + 6);
      vertex.insert(vertex.end(), begin + 15, begin + 18);

      return vertex;
    }

  public:
    /**
     * constructor for single blx
     * @param transform
     * @param scale
     * @param color
     * @param id_program
     */
    Box(const Transform &transform, const Vec &scale, const Vec &color, const GLuint id_program) :
        MeshUnicolor(getVertex(transform, scale), color, id_program)
    {
    }
  };
}

#endif //COMMONVISUALIZER_BOX_HPP

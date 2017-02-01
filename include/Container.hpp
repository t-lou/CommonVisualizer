//
// Created by tlou on 29.01.17.
//

#ifndef COMMONVISUALIZER_CONTAINER_HPP
#define COMMONVISUALIZER_CONTAINER_HPP

#include "Object.hpp"

namespace loco
{
  class Container : public Object
  {
  protected:
    std::vector< std::unique_ptr<Object> > _belongings;

  public:
    Container() : Object(0)
    {
    }

    ~Container()
    {
    }

    void reset()
    {
      _belongings.clear();
    }

    template<class T>
    void addObject(const T *object)
    {
      _belongings.push_back(std::unique_ptr<Object>((Object*)object));
    }

    void display(const glm::mat4 &proj)
    {
      glm::mat4 total_proj = proj * _transform;
      for(const std::unique_ptr<Object> &obj : _belongings)
      {
        obj->display(total_proj);
      }
    }

    void setTransform(const int id, const Transform &transform)
    {
      assert(id < _belongings.size());
      _belongings.at(id)->setTransform(transform);
    }

    void applyTransform(const int id, const Transform &transform)
    {
      assert(id < _belongings.size());
      _belongings.at(id)->applyTransform(transform);
    }
  };
}

#endif //COMMONVISUALIZER_CONTAINER_HPP

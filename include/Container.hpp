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
      for(size_t id = 0; id < _belongings.size(); ++id)
      {
        _belongings.at(id)->display(proj);
      }
    }
  };
}

#endif //COMMONVISUALIZER_CONTAINER_HPP

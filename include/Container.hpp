//
// Created by tlou on 29.01.17.
//

#ifndef COMMONVISUALIZER_CONTAINER_HPP
#define COMMONVISUALIZER_CONTAINER_HPP

#include "Object.hpp"

namespace loco {
/**
 * a class which doesn't contain its own shape, but contains other objects.
 * used for combining simple object to more complex objects.
 * for example a Cylinder consists of one CylinderSide and two OrientedCircle
 */
class Container : public virtual Object {
 protected:
  std::vector<std::unique_ptr<Object>> _belongings;

 public:
  Container() : Object(0) {}

  void reset() { _belongings.clear(); }

  void addObject(std::unique_ptr<Object> object) {
    _belongings.push_back(std::move(object));
  }

  void reserve(const size_t size_space) { _belongings.reserve(size_space); }

  void display(const glm::mat4 &proj) override {
    for (const std::unique_ptr<Object> &obj : _belongings) {
      obj->display(proj);
    }
  }

  void setTransform(const int id, const Transform &transform) {
    assert(id >= 0 && id < _belongings.size());
    _belongings.at(id)->setTransform(transform);
  }

  void applyTransform(const int id, const Transform &transform) {
    assert(id >= 0 && id < _belongings.size());
    _belongings.at(id)->applyTransform(transform);
  }

  void setTransform(const Transform &transform) override {
    for (std::unique_ptr<Object> &obj : _belongings) {
      obj->setTransform(transform);
    }
  }

  void applyTransform(const Transform &transform) override {
    for (std::unique_ptr<Object> &obj : _belongings) {
      obj->applyTransform(transform);
    }
  }
};
}  // namespace loco

#endif  // COMMONVISUALIZER_CONTAINER_HPP

//
// Created by tlou on 18.02.17.
//

#ifndef COMMONVISUALIZER_BULLETVISUALIZER_HPP
#define COMMONVISUALIZER_BULLETVISUALIZER_HPP

#include <btBulletDynamicsCommon.h>

#include <vector>

#include "CommonVisualizer.hpp"

namespace loco {
class BulletVisualizer : public CommonVisualizer {
 protected:
 public:
  BulletVisualizer(const int height = 600, const int width = 800,
                   const char *name = "default",
                   const Vec &background = color::BLACK)
      : CommonVisualizer(height, width, name, background) {}

  ~BulletVisualizer() {}

  void addDiscreteDynamicsWorld(const btDynamicsWorld &world,
                                const std::vector<Vec> &colors) {
    const int length = world.getNumCollisionObjects();
    const Transform tf_zero = Transform{Vec{}, Vec{{0.0f, 0.0f, 0.0f, 1.0f}}};
    assert(length == colors.size());
    for (int ido = 0; ido < length; ++ido) {
      const btRigidBody *body =
          btRigidBody::upcast(world.getCollisionObjectArray()[ido]);
      const Vec &color = colors.at(ido);
      if (strcmp(body->getCollisionShape()->getName(), "Box") == 0) {
        btVector3 size = ((btBoxShape *)body->getCollisionShape())
                             ->getHalfExtentsWithMargin();
        addBox(
            tf_zero,
            Vec{{float(size.getX()), float(size.getY()), float(size.getZ())}},
            color);
      } else if (strcmp(body->getCollisionShape()->getName(), "Sphere") == 0) {
        float radius =
            float(((btSphereShape *)body->getCollisionShape())->getRadius());
        addPointCloud(std::vector<float>(3, 0.0f), color, radius);
      } else if (strcmp(body->getCollisionShape()->getName(), "CapsuleShape") ==
                 0) {
        const btCapsuleShape *shape =
            (btCapsuleShape *)body->getCollisionShape();
        float radius = float(shape->getRadius());
        int id_up = shape->getUpAxis();
        float half_height = float(shape->getHalfHeight());
        std::vector<float> pos(6, 0.0f);
        pos.at(id_up) = half_height;
        pos.at(id_up + 3) = -half_height;
        addCapsule(pos, std::vector<float>(1, radius),
                   std::vector<Vec>(1, color));
      }
      //        else if(strcmp(body->getCollisionShape()->getName(),
      //        "CylinderShape") == 0) // TODO is the name right?
      //        {
      //          const btCylinderShape *shape = (btCapsuleShape *)
      //          body->getCollisionShape(); float radius =
      //          float(shape->getRadius()); int id_up = shape->getUpAxis();
      //          float half_height = float(shape->getHalfHeight());
      //          std::vector<float> pos(6, 0.0f);
      //          pos.at(id_up) = half_height;
      //          pos.at(id_up + 3) = -half_height;
      //          addCylinder(pos, std::vector<float>(1, radius),
      //          std::vector<Vec>(1, color));
      //        }
      // TODO missing cone, irregular shapes and maybe softbody
    }
  }

  void addDiscreteDynamicsWorld(const btDynamicsWorld &world,
                                const Vec &color = color::WHITE) {
    addDiscreteDynamicsWorld(
        world, std::vector<Vec>(world.getNumCollisionObjects(), color));
  }

  void updateTransformDiscreteDynamicsWorld(const btDynamicsWorld &world) {
    const int length = world.getNumCollisionObjects();
    for (int ido = 0; ido < length; ++ido) {
      btCollisionObject *obj = world.getCollisionObjectArray()[ido];
      btRigidBody *body = btRigidBody::upcast(obj);
      btTransform tf;

      body->getMotionState()->getWorldTransform(tf);
      btVector3 translation = tf.getOrigin();
      btQuaternion rotation = tf.getRotation();
      setTransform(
          ido,
          Transform{Vec{{float(translation.getX()), float(translation.getY()),
                         float(translation.getZ())}},
                    Vec{{float(rotation.getX()), float(rotation.getY()),
                         float(rotation.getZ()), float(rotation.getW())}}});
    }
  }
};
}  // namespace loco

#endif  // COMMONVISUALIZER_BULLETVISUALIZER_HPP

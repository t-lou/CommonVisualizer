/**
 * One of motivations of CommonVisulizer is to visualize result of other programs in a convenient way,
 * for example bullet physics engine, whose visualization requires self implemeted drawLine... why bother doing it
 * 
 * This scene is about boxes falling into a crate, which was part of my old code about creating point cloud 
 * of crates with boxes. Less than 20 lines of code is necessary for visualization.
 * 
 * Here a function calls function of CommonVisualizer is used. In the future special classes for bullet and pcl may be created
 */

#include <iostream>
#include <vector>
#include <memory>
#include <cmath>
#include <cstdlib>
#include <unistd.h>

#include <btBulletDynamicsCommon.h>
#include <BulletCollision/NarrowPhaseCollision/btRaycastCallback.h>
#include <BulletCollision/Gimpact/btGImpactShape.h>

#include "CommonVisualizer.hpp"

// how to compile (assuming bullet and all gl libraries installed)
// root directory of CommonVisualizer is /home/tlou/workspace/CommonVisualizer/ on my laptop
// clang++ test_bullet_crate.cpp -o test_bullet_crate -I/usr/include/bullet/ -I/home/tlou/workspace/CommonVisualizer/ -lBulletDynamics -lBulletCollision -lLinearMath -lGL -lGLEW -lGLU -lglut -lglfw --std=c++11

const double scaling = 1.0;
std::unique_ptr<loco::CommonVisualizer> vis;

btDiscreteDynamicsWorld init_world()
{
    btDefaultCollisionConfiguration *config = new btDefaultCollisionConfiguration();
    btDiscreteDynamicsWorld world(new btCollisionDispatcher(config),
                                  (btBroadphaseInterface*)new btDbvtBroadphase(),
                                  new btSequentialImpulseConstraintSolver(),
                                  config);
    world.setGravity(btVector3(0.0, 0.0, -9.8) * scaling);
    return world;
}

void add_ground(const btVector3 &scaling,
                const btVector3 &center,
                const btQuaternion &rotation,
                btDiscreteDynamicsWorld &world)
{
    btScalar mass(0.0);
    btVector3 inertia(0.0,0.0,0.0);
    btCollisionShape *ground = new btBoxShape(scaling);
    btTransform tf_ground;
    tf_ground.setIdentity();
    tf_ground.setOrigin(center);
    tf_ground.setRotation(rotation);
    world.addRigidBody(new btRigidBody(
        btRigidBody::btRigidBodyConstructionInfo(
            mass, new btDefaultMotionState(tf_ground),
            ground, inertia)));
}

void add_box(const btVector3 &scaling,
             const btVector3 &center,
             const btQuaternion &rotation,
             const btScalar &mass,
             btDiscreteDynamicsWorld &world)
{
    btVector3 inertia(0.0, 0.0, 0.0);
    btCollisionShape *this_ = new btBoxShape(scaling);
    btTransform start_tf;
    start_tf.setIdentity();
    start_tf.setOrigin(center);
    start_tf.setRotation(rotation);
    this_->calculateLocalInertia(mass, inertia);
    world.addRigidBody(new btRigidBody(btRigidBody::btRigidBodyConstructionInfo(
            mass, new btDefaultMotionState(start_tf), this_, inertia)));
}

void add_box_vis(const btVector3 &size, const btVector3 &translation, const btQuaternion &rotation, const loco::Vec &color)
{
  // btVector3 size = box.getHalfExtentsWithMargin();
  // btVector3 translation = box.getWorldTransform().getOrigin();
  // btQuaternion rotation = box.getWorldTransform().getRotation();
  vis->addBox(loco::Transform{loco::Vec{float(translation.getX()), float(translation.getY()), float(translation.getZ())}, 
                                loco::Vec{float(rotation.getX()), float(rotation.getY()), float(rotation.getZ()), float(rotation.getW())}}, 
                loco::Vec{float(size.getX()), float(size.getY()), float(size.getZ())}, color);
}

void run(const float interval,
         const int iteration,
         btDynamicsWorld &world)
{
    for(int iter = 0; iter < iteration; ++iter)
    {
      vis->resetScene();
        world.stepSimulation(interval);
        for(int ido = 0; ido < world.getNumCollisionObjects(); ++ido)
        {
            btCollisionObject *obj = world.getCollisionObjectArray()[ido];
            btRigidBody *body = btRigidBody::upcast(obj);
            btTransform tf;
            if(body && body->getMotionState())
            {
                body->getMotionState()->getWorldTransform(tf);
            }
            else
            {
                tf = obj->getWorldTransform();
            }
            loco::Vec color = ido < 61 ? loco::color::BLUE : loco::color::WHITE;
            add_box_vis(((btBoxShape*)body->getCollisionShape())->getHalfExtentsWithMargin(),
                      tf.getOrigin(), tf.getRotation(), color);
        }
        vis->playOnce();
        usleep(interval * 1e6f);
    }
}

void set_crate(btDiscreteDynamicsWorld &world)
{
    add_ground(btVector3(0.3, 0.2, 0.001) * scaling,
               btVector3(0.0, 0.0, -0.001) * scaling,
               btQuaternion(0, 0, 0), world);
    add_ground(btVector3(0.3, 0.15, 0.01) * scaling,
               btVector3(0.0, 0.21, 0.15) * scaling,
               btQuaternion(0, SIMD_PI / 2.0, 0), world);
    add_ground(btVector3(0.3, 0.15, 0.01) * scaling,
               btVector3(0.0, -0.21, 0.15) * scaling,
               btQuaternion(0, -SIMD_PI / 2.0, 0), world);
    add_ground(btVector3(0.15, 0.22, 0.01) * scaling,
               btVector3(0.31, 0.0, 0.15) * scaling,
               btQuaternion(SIMD_PI / 2.0, 0, 0), world);
    add_ground(btVector3(0.15, 0.22, 0.01) * scaling,
               btVector3(-0.31, -0.0, 0.15) * scaling,
               btQuaternion(-SIMD_PI / 2.0, 0, 0), world);

    // details dir 1
    add_ground(btVector3(0.033125, 0.15, 0.0025) * scaling,
               btVector3(0.266875, 0.1975, 0.15) * scaling,
               btQuaternion(0, SIMD_PI / 2.0, 0), world);
    add_ground(btVector3(0.033125, 0.15, 0.0025) * scaling,
               btVector3(0.190625, 0.1975, 0.15) * scaling,
               btQuaternion(0, SIMD_PI / 2.0, 0), world);
    add_ground(btVector3(0.033125, 0.15, 0.0025) * scaling,
               btVector3(0.114375, 0.1975, 0.15) * scaling,
               btQuaternion(0, SIMD_PI / 2.0, 0), world);
    add_ground(btVector3(0.033125, 0.15, 0.0025) * scaling,
               btVector3(0.038125, 0.1975, 0.15) * scaling,
               btQuaternion(0, SIMD_PI / 2.0, 0), world);
    add_ground(btVector3(0.033125, 0.15, 0.0025) * scaling,
               btVector3(-0.038125, 0.1975, 0.15) * scaling,
               btQuaternion(0, SIMD_PI / 2.0, 0), world);
    add_ground(btVector3(0.033125, 0.15, 0.0025) * scaling,
               btVector3(-0.114375, 0.1975, 0.15) * scaling,
               btQuaternion(0, SIMD_PI / 2.0, 0), world);
    add_ground(btVector3(0.033125, 0.15, 0.0025) * scaling,
               btVector3(-0.190625, 0.1975, 0.15) * scaling,
               btQuaternion(0, SIMD_PI / 2.0, 0), world);
    add_ground(btVector3(0.033125, 0.15, 0.0025) * scaling,
               btVector3(-0.266875, 0.1975, 0.15) * scaling,
               btQuaternion(0, SIMD_PI / 2.0, 0), world);

    // details dir 2
    add_ground(btVector3(0.033125, 0.15, 0.0025) * scaling,
               btVector3(0.266875, -0.1975, 0.15) * scaling,
               btQuaternion(0, -SIMD_PI / 2.0, 0), world);
    add_ground(btVector3(0.033125, 0.15, 0.0025) * scaling,
               btVector3(0.190625, -0.1975, 0.15) * scaling,
               btQuaternion(0, -SIMD_PI / 2.0, 0), world);
    add_ground(btVector3(0.033125, 0.15, 0.0025) * scaling,
               btVector3(0.114375, -0.1975, 0.15) * scaling,
               btQuaternion(0, -SIMD_PI / 2.0, 0), world);
    add_ground(btVector3(0.033125, 0.15, 0.0025) * scaling,
               btVector3(0.038125, -0.1975, 0.15) * scaling,
               btQuaternion(0, -SIMD_PI / 2.0, 0), world);
    add_ground(btVector3(0.033125, 0.15, 0.0025) * scaling,
               btVector3(-0.038125, -0.1975, 0.15) * scaling,
               btQuaternion(0, -SIMD_PI / 2.0, 0), world);
    add_ground(btVector3(0.033125, 0.15, 0.0025) * scaling,
               btVector3(-0.114375, -0.1975, 0.15) * scaling,
               btQuaternion(0, -SIMD_PI / 2.0, 0), world);
    add_ground(btVector3(0.033125, 0.15, 0.0025) * scaling,
               btVector3(-0.190625, -0.1975, 0.15) * scaling,
               btQuaternion(0, -SIMD_PI / 2.0, 0), world);
    add_ground(btVector3(0.033125, 0.15, 0.0025) * scaling,
               btVector3(-0.266875, -0.1975, 0.15) * scaling,
               btQuaternion(0, -SIMD_PI / 2.0, 0), world);

    // details dir 3
    add_ground(btVector3(0.15, 0.0425, 0.0025) * scaling,
               btVector3(0.2975, 0.1475, 0.15) * scaling,
               btQuaternion(SIMD_PI / 2.0, 0, 0), world);
    add_ground(btVector3(0.15, 0.045, 0.0025) * scaling,
               btVector3(0.2975, 0.05, 0.15) * scaling,
               btQuaternion(SIMD_PI / 2.0, 0, 0), world);
    add_ground(btVector3(0.15, 0.045, 0.0025) * scaling,
               btVector3(0.2975, -0.05, 0.15) * scaling,
               btQuaternion(SIMD_PI / 2.0, 0, 0), world);
    add_ground(btVector3(0.15, 0.0425, 0.0025) * scaling,
               btVector3(0.2975, -0.1475, 0.15) * scaling,
               btQuaternion(SIMD_PI / 2.0, 0, 0), world);

    // details dir 4
    add_ground(btVector3(0.15, 0.0425, 0.0025) * scaling,
               btVector3(-0.2975, 0.1475, 0.15) * scaling,
               btQuaternion(-SIMD_PI / 2.0, 0, 0), world);
    add_ground(btVector3(0.15, 0.045, 0.0025) * scaling,
               btVector3(-0.2975, 0.05, 0.15) * scaling,
               btQuaternion(-SIMD_PI / 2.0, 0, 0), world);
    add_ground(btVector3(0.15, 0.045, 0.0025) * scaling,
               btVector3(-0.2975, -0.05, 0.15) * scaling,
               btQuaternion(-SIMD_PI / 2.0, 0, 0), world);
    add_ground(btVector3(0.15, 0.0425, 0.0025) * scaling,
               btVector3(-0.2975, -0.1475, 0.15) * scaling,
               btQuaternion(-SIMD_PI / 2.0, 0, 0), world);

    // detail ground
    for(int i = 0; i < 8; ++i)
        for(int j = 0; j < 4; ++j)
        {
            float step_x = (0.6 - 0.08) / 8.0 + 0.01;
            float step_y = (0.4 - 0.04) / 4.0 + 0.01;
            float offset_x = -0.2995 + (0.6 - 0.08) / 16.0;
            float offset_y = -0.1995 + (0.4 - 0.04) / 8.0;
            add_ground(btVector3((0.6 - 0.08) / 16.0, (0.4 - 0.04) / 8.0, 0.0025) * scaling,
                       btVector3(offset_x + step_x * i, offset_y + step_y * j, 0.0025) * scaling,
                       btQuaternion(0, 0, 0), world);
        }
}

void add_boxes(btDiscreteDynamicsWorld &world)
{
    add_box(btVector3(0.025, 0.03, 0.065) * scaling,
            btVector3(0.0, 0.003, 0.3) * scaling,
            btQuaternion(SIMD_PI / 2.0, SIMD_PI / 4.0, 0), btScalar(1.0f), world);
    add_box(btVector3(0.025, 0.03, 0.065) * scaling,
            btVector3(0.0, 0.0015, 0.1) * scaling,
            btQuaternion(0, 0, SIMD_PI / 1.2), btScalar(1.0), world);
    add_box(btVector3(0.025, 0.03, 0.065) * scaling,
            btVector3(0.0, 0.0015, 0.6) * scaling,
            btQuaternion(0, 0, SIMD_PI / 1.2), btScalar(1.0), world);
    add_box(btVector3(0.025, 0.03, 0.065) * scaling,
            btVector3(0.0, 0.0015, 0.7) * scaling,
            btQuaternion(0, 0, SIMD_PI / 1.2), btScalar(1.0), world);
    add_box(btVector3(0.025, 0.03, 0.065) * scaling,
            btVector3(0.0, 0.0015, 0.8) * scaling,
            btQuaternion(0, 0, SIMD_PI / 1.2), btScalar(1.0), world);
    add_box(btVector3(0.025, 0.03, 0.065) * scaling,
            btVector3(0.0, 0.0015, 0.9) * scaling,
            btQuaternion(0, 0, SIMD_PI / 1.2), btScalar(1.0), world);
    add_box(btVector3(0.025, 0.03, 0.065) * scaling,
            btVector3(0.0, 0.003, 0.4) * scaling,
            btQuaternion(SIMD_PI / 2.0, SIMD_PI / 4.0, 0), btScalar(1.0f), world);
}

int main(int argc, char **argv)
{
//    btAlignedObjectArray<btCollisionShape*> shape_list;
  vis.reset(new loco::CommonVisualizer(600, 800, "crate", loco::color::BLACK));
  vis->setDistance(2.0f);
  vis->setLightSource(loco::Vec{0.0f, 0.0f, 1.0f}, loco::color::WHITE);
  vis->setTheta(M_PI / 6.0);
  if(vis->isInited())
    {
      btDiscreteDynamicsWorld world = init_world();
      set_crate(world);
      add_boxes(world);

      run(btScalar(0.01), 200, world);
      vis->play();
    }
    else
    {
      return 1;
    }

    return 0;
}


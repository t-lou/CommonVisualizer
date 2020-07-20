cmake_minimum_required(VERSION 3.6)
project(Examples)

set(CMAKE_CXX_STANDARD 14)
set(CMAKE_BUILD_TYPE Release)

find_package(Eigen3 REQUIRED)

include_directories(
  ..
  /usr/include/bullet
  ${EIGEN3_INCLUDE_DIRS}
)

add_executable(test_all test_all.cpp)
target_link_libraries(test_all 
  GL GLEW GLU glut glfw 
  ${EIGEN3_LIBRARIES}
)

add_executable(test_bullet_crate test_bullet_crate.cpp)
target_link_libraries(test_bullet_crate 
  GL GLEW GLU glut glfw
  BulletDynamics BulletCollision LinearMath
)

add_executable(test_spheres test_spheres.cpp)
target_link_libraries(test_spheres 
  GL GLEW GLU glut glfw
)

add_executable(ply_viewer ply_viewer.cpp)
target_link_libraries(ply_viewer GL GLEW GLU glut glfw)

add_executable(test_screensaver test_screensaver.cpp)
target_link_libraries(test_screensaver GL GLEW GLU glut glfw png)

add_executable(test_sep test_sep_main.cpp test_sep_part.cpp)
target_link_libraries(test_sep
  GL GLEW GLU glut glfw
  ${EIGEN3_LIBRARIES}
)
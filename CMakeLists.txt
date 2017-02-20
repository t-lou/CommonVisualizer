cmake_minimum_required(VERSION 3.6)
project(CommonVisualizer)

set(CMAKE_CXX_STANDARD 11)

find_package(Eigen3 REQUIRED)
include_directories(/usr/include/bullet ${EIGEN3_INCLUDE_DIRS})

add_executable(Test test.cpp)
target_link_libraries(Test GL GLEW GLU glut glfw ${EIGEN3_LIBRARIES})
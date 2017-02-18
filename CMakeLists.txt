cmake_minimum_required(VERSION 3.6)
project(CommonVisualizer)

set(CMAKE_CXX_STANDARD 11)

#include_directories(/usr/include/bullet)

add_executable(Test test.cpp)
target_link_libraries(Test GL GLEW GLU glut glfw)
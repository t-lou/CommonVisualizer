cmake_minimum_required(VERSION 3.6)
project(CommonVisualizer)

set(CMAKE_CXX_STANDARD 11)

add_executable(Test test.cpp)
target_link_libraries(Test GL GLEW GLU glut glfw)
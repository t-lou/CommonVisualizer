//
// Created by tlou on 20.01.17.
//

#ifndef COMMONVISUALIZER_COMMONVISUALIZER_HPP
#define COMMONVISUALIZER_COMMONVISUALIZER_HPP

#include <cmath>
#include <fstream>
#include <iostream>
#include <numeric>
#include <queue>

#include "CommonVisualizer.h"
#include "include/Arrow.hpp"
#include "include/Box.hpp"
#include "include/Capsule.hpp"
#include "include/CloudPointColored.hpp"
#include "include/CloudPointUnicolor.hpp"
#include "include/CloudSphereColored.hpp"
#include "include/CloudSphereUnicolor.hpp"
#include "include/Cone.hpp"
#include "include/CoordinateUnits.hpp"
#include "include/Cylinder.hpp"
#include "include/MeshColored.hpp"
#include "include/MeshUnicolor.hpp"

namespace loco {
/**
 * print error by compilation
 * @param length
 * @param id
 */
void CommonVisualizer::dispErrorCompilation(const int length, const GLuint id,
                                            const GLenum &type) {
  std::string error;
  error.resize(length + 1);
  switch (type) {
    case GL_PROGRAM:
      glGetProgramInfoLog(id, length, NULL, &error.front());
      break;
    case GL_SHADER:
      glGetShaderInfoLog(id, length, NULL, &error.front());
      break;
  }
  std::cout << error << std::endl;
}

/**
 * load all text from a document. designed to load shader, but I want the
 * program to be independant to source at run time
 * @param filename
 * @return
 */
std::string CommonVisualizer::loadSource(const std::string &filename) {
  std::ifstream in(filename);
  in.seekg(0, in.end);

  int length = in.tellg();
  std::string source;
  source.reserve(length);
  in.seekg(0, in.beg);
  in.read(&source.front(), length);

  return source;
}

/**
 * create single shader with source code shader_text and shader type(vertex,
 * fragement...)
 * @param shader_type
 * @param shader_text
 * @return
 */
GLuint CommonVisualizer::createShader(const GLenum &shader_type,
                                      const char *shader_text) {
  GLuint id_shader = glCreateShader(shader_type);
  GLint re = GL_FALSE;
  int len_info;
  glShaderSource(id_shader, 1, &shader_text, NULL);
  glCompileShader(id_shader);
  glGetShaderiv(id_shader, GL_COMPILE_STATUS, &re);
  glGetShaderiv(id_shader, GL_INFO_LOG_LENGTH, &len_info);
  if (len_info > 0) {
    std::cout << "error in shader:" << std::endl;
    std::cout << std::string(shader_text) << std::endl;
    dispErrorCompilation(len_info, id_shader, GL_SHADER);
  }
  return id_shader;
}

/**
 * create opengl program with vector of shaders and source codes
 * @param shader_types
 * @param shader_texts
 * @return
 */
GLuint CommonVisualizer::createProgram(
    const std::vector<GLenum> &shader_types,
    const std::vector<const char *> &shader_texts) {
  assert(shader_types.size() == shader_texts.size());
  GLuint id_program;
  GLint re = GL_FALSE;
  int len_info;
  std::vector<GLuint> id_shaders;
  id_shaders.reserve(shader_texts.size());

  // create each shader
  for (size_t id = 0; id < shader_texts.size(); ++id) {
    id_shaders.push_back(CommonVisualizer::createShader(shader_types.at(id),
                                                        shader_texts.at(id)));
  }
  // attach to program
  id_program = glCreateProgram();
  for (GLuint &id_shader : id_shaders) {
    glAttachShader(id_program, id_shader);
  }
  glLinkProgram(id_program);
  glGetProgramiv(id_program, GL_LINK_STATUS, &re);
  glGetProgramiv(id_program, GL_INFO_LOG_LENGTH, &len_info);
  if (len_info > 0) {
    std::cout << "error by program" << std::endl;
    dispErrorCompilation(len_info, id_program, GL_PROGRAM);
  }
  // detach and release shaders
  for (GLuint &id_shader : id_shaders) {
    glDetachShader(id_program, id_shader);
    glDeleteShader(id_shader);
  }
  return id_program;
}

/**
 * update position of viewer (eye) with external transform
 */
void CommonVisualizer::updatePosViewer() {
  const float sin_theta{std::sin(_theta)};
  _pos_viewer =
      _transform_camera * glm::vec4(_distance * sin_theta * std::sin(_phi),
                                    _distance * sin_theta * std::cos(_phi),
                                    _distance * std::cos(_theta), 1.0f);
}

/**
 * update direction of up with external transform
 */
void CommonVisualizer::updateUpViewer() {
  constexpr float half_pi{glm::radians(90.0f)};
  const float theta = _theta - half_pi;
  const float sin_theta{std::sin(theta)};
  _up_viewer = _transform_camera * glm::vec4{sin_theta * std::sin(-_phi),
                                             sin_theta * std::cos(-_phi),
                                             std::cos(theta), 0.0f};
}

/**
 * update center (where to look at) with external transform
 */
void CommonVisualizer::updateCenterViewer() {
  _center_viewer = _transform_camera * glm::vec4(0.0f, 0.0f, 0.0f, 1.0f);
}

/**
 * update viewing matrix with position, look at position and up direction of
 * viewer (class member)
 */
void CommonVisualizer::updateViewingMatrix() {
  updatePosViewer();
  updateCenterViewer();
  updateUpViewer();
  _mat_view = glm::lookAt(glm::vec3{_pos_viewer}, glm::vec3{_center_viewer},
                          glm::vec3{_up_viewer});
}

/**
 * rotate and zoom(not implemented) according to mouse input
 */
void CommonVisualizer::onRotationAndZooming() {
  if (!_interaction._is_left_pressed &&
      glfwGetMouseButton(_window, GLFW_MOUSE_BUTTON_LEFT) == GLFW_PRESS) {
    glfwGetCursorPos(_window, &_interaction._xpos_prev,
                     &_interaction._ypos_prev);
    _interaction._is_left_pressed = true;
  } else if (_interaction._is_left_pressed &&
             glfwGetMouseButton(_window, GLFW_MOUSE_BUTTON_LEFT) ==
                 GLFW_RELEASE) {
    double xpos = 0.0, ypos = 0.0;
    glfwGetCursorPos(_window, &xpos, &ypos);
    float dx = (float)(xpos - _interaction._xpos_prev);
    float dy = (float)(ypos - _interaction._ypos_prev);

    // if the mouse in one direction is dominant, ignore the change in the other
    // direction
    if (std::abs(dx) > 3.0f * std::abs(dy)) {
      dy = 0.0f;
    } else if (std::abs(dx) * 3.0f < std::abs(dy)) {
      dx = 0.0f;
    }
    const static float theta_scaling = (float)(M_PI * 0.25) / (float)_height;
    const static float phi_scaling = (float)(M_PI * 0.25) / (float)_width;
    _theta -= dy * theta_scaling;
    _phi += dx * phi_scaling;
    _theta = std::min((float)M_PI, std::max(0.0f, _theta));

    updateViewingMatrix();
    _interaction._is_left_pressed = false;
  } else if (!_interaction._is_middle_pressed &&
             glfwGetMouseButton(_window, GLFW_MOUSE_BUTTON_MIDDLE) ==
                 GLFW_PRESS) {
    glfwGetCursorPos(_window, &_interaction._xpos_prev,
                     &_interaction._ypos_prev);
    _interaction._is_middle_pressed = true;
  } else if (_interaction._is_middle_pressed &&
             glfwGetMouseButton(_window, GLFW_MOUSE_BUTTON_MIDDLE) ==
                 GLFW_RELEASE) {
    double xpos = 0.0, ypos = 0.0;
    glfwGetCursorPos(_window, &xpos, &ypos);
    const double dy = ypos - _interaction._ypos_prev;
    _distance *= (float)pow(1.1, dy / 50.0);
    updateViewingMatrix();
    _interaction._is_middle_pressed = false;
  } else if (!_interaction._is_right_pressed &&
             glfwGetMouseButton(_window, GLFW_MOUSE_BUTTON_RIGHT) ==
                 GLFW_PRESS) {
    glfwGetCursorPos(_window, &_interaction._xpos_prev,
                     &_interaction._ypos_prev);
    _interaction._is_right_pressed = true;
  } else if (_interaction._is_right_pressed &&
             glfwGetMouseButton(_window, GLFW_MOUSE_BUTTON_RIGHT) ==
                 GLFW_RELEASE) {
    double xpos = 0.0, ypos = 0.0;
    glfwGetCursorPos(_window, &xpos, &ypos);
    float dx = (float)(xpos - _interaction._xpos_prev);
    float dy = (float)(ypos - _interaction._ypos_prev);

    const glm::vec3 up_dir{_up_viewer};
    const glm::vec3 left_dir{glm::cross(
        up_dir,
        glm::normalize(glm::vec3{_center_viewer} - glm::vec3{_pos_viewer}))};
    _transform_camera[3] += glm::vec4(up_dir * dy / 1000.0f, 0.0f);
    _transform_camera[3] += glm::vec4(left_dir * dx / 1000.0f, 0.0f);

    updateViewingMatrix();
    _interaction._is_right_pressed = false;
  }
}

/**
 * update viewer pose in program
 * @param id_program
 */
void CommonVisualizer::updateViewerPose(const GLuint &id_program) {
  GLuint id_up = glGetUniformLocation(id_program, "dir_up");
  GLuint id_eye = glGetUniformLocation(id_program, "pos_eye");
  glUseProgram(id_program);
  glUniform3fv(id_up, 1, &_up_viewer[0]);
  glUniform3fv(id_eye, 1, &_pos_viewer[0]);
}

/**
 * prepare parameters for phong lighting to program
 * @param id_program
 */
void CommonVisualizer::updatePhongParameter(const GLuint &id_program) {
  GLuint id_pos_source = glGetUniformLocation(id_program, "pos_light");
  GLuint id_color_source = glGetUniformLocation(id_program, "color_light");
  GLuint id_pos_viewer = glGetUniformLocation(id_program, "pos_eye");
  GLuint id_param_phong = glGetUniformLocation(id_program, "phong");
  glUseProgram(id_program);
  glUniform3fv(id_pos_source, 1, _light_source._pos._data);
  glUniform4fv(id_color_source, 1, _light_source._color._data);
  glUniform3fv(id_pos_viewer, 1, &_pos_viewer[0]);
  glUniform4fv(id_param_phong, 1, _param_phong._data);
}

/**
 * update phong parameter in all created programs
 */
void CommonVisualizer::updatePhongParameter() {
  for (GLuint &id_prog : _id_prog._ids) {
    if (id_prog) {
      updatePhongParameter(id_prog);
    }
  }

  if (_id_prog._id_program_unicolor_cloud_sphere) {
    updateViewerPose(_id_prog._id_program_unicolor_cloud_sphere);
  }
  if (_id_prog._id_program_colored_cloud_sphere) {
    updateViewerPose(_id_prog._id_program_colored_cloud_sphere);
  }
  if (_id_prog._id_program_cylinder_side) {
    updateViewerPose(_id_prog._id_program_cylinder_side);
  }
  if (_id_prog._id_program_oriented_circle) {
    updateViewerPose(_id_prog._id_program_oriented_circle);
  }
  if (_id_prog._id_program_cone_side) {
    updateViewerPose(_id_prog._id_program_cone_side);
  }
}

CommonVisualizer::CommonVisualizer(const int height, const int width,
                                   const char *name, const Vec &background)
    : _height(height),
      _width(width),
      _re_init(0),
      _window(NULL),
      _distance(3.0f),
      _theta(glm::radians(90.0f)),
      _phi(0.0f),
      _mat_proj(glm::perspective(
          glm::radians(45.0f), (float)_width / (float)_height, 0.1f, 5000.0f)),
      _transform_camera(Object::transformToMat4(
          Transform{Vec{0.0f, 0.0f, 0.0f}, Vec{0.0f, 0.0f, 0.0f, 1.0f}})),
      _param_phong(Vec{0.3f, 0.3f, 0.4f, 2.0f}) {
  // glfw init
  _re_init = (glfwInit() == GLFW_TRUE);
  if (!_re_init) {
    return;
  }
  glfwWindowHint(GLFW_SAMPLES, 4);
  glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
  glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
  glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
  glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

  // window init
  _window = glfwCreateWindow(_width, _height, name, NULL, NULL);
  if (!_window) {
    return;
  }
  glfwMakeContextCurrent(_window);

  // glew init
  glewExperimental = GL_TRUE;
  _re_init = (glewInit() == GLEW_OK);  // before is true, same as and
  if (!_re_init) {
    return;
  }

  glfwSetInputMode(_window, GLFW_STICKY_MOUSE_BUTTONS, GL_TRUE);

  glClearColor(background._r, background._g, background._b, background._a);
  glEnable(GL_DEPTH_TEST);
  glDepthFunc(GL_LESS);
  glEnable(GL_CULL_FACE);  // backface culling
  glEnable(GL_PROGRAM_POINT_SIZE);
  glEnable(GL_BLEND);
  glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

  for (GLuint &id_prog : _id_prog._ids) {
    id_prog = 0u;
  }

  updateViewingMatrix();
}

CommonVisualizer::~CommonVisualizer() {
  for (GLuint &id_prog : _id_prog._ids) {
    if (id_prog) {
      glDeleteProgram(id_prog);
    }
    id_prog = 0u;
  }
  glfwTerminate();
}

/**
 * clear all objects
 */
void CommonVisualizer::resetScene() { _world.reset(); }

/**
 * set distance from eye to center (zooming)
 * @param distance
 */
void CommonVisualizer::setDistance(const float distance) {
  _distance = distance;
  updateViewingMatrix();
}

/**
 * set theta of viewer, which is the angle of viewing direction to -Z
 * @param theta
 */
void CommonVisualizer::setTheta(const float theta) {
  _theta = theta;
  updateViewingMatrix();
}

/**
 * set phi of view, which is rotation around Z-axis
 * @param phi
 */
void CommonVisualizer::setPhi(const float phi) {
  _phi = phi;
  updateViewingMatrix();
}

/**
 * displace once
 * @return whether to continue
 */
bool CommonVisualizer::playOnce() {
  glfwSetTime(0.0);

  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
  onRotationAndZooming();
  glm::mat4 proj = _mat_proj * _mat_view;  // without its own tf

  updatePhongParameter();
  _world.display(proj);

  glfwSwapBuffers(_window);
  glfwPollEvents();

  static std::deque<double> intervals;
  const std::size_t limit{10u};
  intervals.push_back(glfwGetTime());
  if (intervals.size() > limit) {
    intervals.pop_front();
  }
  std::cout << "fps: "
            << (double)intervals.size() /
                   std::accumulate(intervals.begin(), intervals.end(), 0.0)
            << std::endl;

  return glfwGetKey(_window, GLFW_KEY_ESCAPE) != GLFW_PRESS &&
         glfwWindowShouldClose(_window) == 0;
}

/**
 * render in loop
 */
void CommonVisualizer::play() {
  while (playOnce()) {
  }
}

/**
 * checks intialiazation
 * @return whether the initialization is correct
 */
bool CommonVisualizer::isInited() const { return _re_init && _window != NULL; }

/**
 * add point light source (other are not necessary, point source shows the shape
 * well)
 * @param pos
 * @param color
 */
void CommonVisualizer::setLightSource(const Vec &pos, const Vec &color) {
  _light_source._pos = pos;
  _light_source._color = color;
}

/**
 * move camera with translation and rotation
 * @param transform
 */
void CommonVisualizer::setTransformCamera(const Transform &transform) {
  _transform_camera = Object::transformToMat4(transform);
  updateViewingMatrix();
}

void CommonVisualizer::setTransform(const Transform &transform) {
  _world.setTransform(transform);
}

void CommonVisualizer::setTransform(const int id, const Transform &transform) {
  _world.setTransform(id, transform);
}

/**
 * add mesh with uniform color
 * @param vertices list of vertex, every 9 floats is a trangle
 * @param color uniform color for this mesh
 */
void CommonVisualizer::addMesh(const std::vector<float> &vertices,
                               const Vec &color) {
  if (_id_prog._id_program_unicolor_mesh == 0) {
    loadUnicolorMeshShader();
  }
  _world.addObject(
      new MeshUnicolor(vertices, color, _id_prog._id_program_unicolor_mesh));
}

/**
 * add mesh with vertices and indices for triangles
 * @param vertices
 * @param index
 * @param color
 */
void CommonVisualizer::addMesh(const std::vector<float> &vertices,
                               const std::vector<int> &index,
                               const Vec &color) {
  if (_id_prog._id_program_unicolor_mesh == 0) {
    loadUnicolorMeshShader();
  }
  _world.addObject(new MeshUnicolor(vertices, index, color,
                                    _id_prog._id_program_unicolor_mesh));
}

/**
 * add mesh with color
 * @param vertices list of vertex, every 9 floats is a trangle
 * @param colors list of color value, every 4 floats color on a vertex
 */
void CommonVisualizer::addMesh(const std::vector<float> &vertices,
                               const std::vector<float> &colors) {
  if (_id_prog._id_program_colored_mesh == 0) {
    loadColoredMeshShader();
  }
  _world.addObject(
      new MeshColored(vertices, colors, _id_prog._id_program_colored_mesh));
}

/**
 *
 * @param points
 * @param colors
 * @param radius
 */
void CommonVisualizer::addPointCloud(const std::vector<float> &points,
                                     const std::vector<float> &colors,
                                     const float radius) {
  if (radius <= 0.0f) {
    if (_id_prog._id_program_colored_cloud_point == 0) {
      loadColoredCloudPointShader();
    }
    _world.addObject(new CloudPointColored(
        points, colors, -radius, _id_prog._id_program_colored_cloud_point));
  } else {
    if (_id_prog._id_program_colored_cloud_sphere == 0) {
      loadColoredCloudSphereShader();
    }
    _world.addObject(new CloudSphereColored(
        points, colors, radius, _id_prog._id_program_colored_cloud_sphere));
  }
}

void CommonVisualizer::addPointCloud(const std::vector<float> &points,
                                     const Vec &color, const float radius) {
  if (radius <= 0.0f) {
    if (_id_prog._id_program_unicolor_cloud_point == 0) {
      loadUnicolorCloudPointShader();
    }
    _world.addObject(new CloudPointUnicolor(
        points, color, -radius, _id_prog._id_program_unicolor_cloud_point));
  } else {
    if (_id_prog._id_program_unicolor_cloud_sphere == 0) {
      loadUnicolorCloudSphereShader();
    }
    _world.addObject(new CloudSphereUnicolor(
        points, color, radius, _id_prog._id_program_unicolor_cloud_sphere));
  }
}

/**
 * add the notation for coordinate system after transform
 * @param transform
 */
void CommonVisualizer::addCoordinateSign(const Transform &transform) {
  if (_id_prog._id_program_colored_cloud_point == 0) {
    loadColoredCloudPointShader();
  }
  _world.addObject(
      new CoordinateUnits(_id_prog._id_program_colored_cloud_point));
}

/**
 * add cylinder to visualizer
 * @param positions vector of positions of length 6*num, for each arrow:
 *        from_x, from_y, from_z, to_x, to_y, to_z
 * @param radius
 * @param colors
 */
void CommonVisualizer::addCylinder(const std::vector<float> &positions,
                                   const std::vector<float> &radius,
                                   const std::vector<Vec> &colors) {
  if (_id_prog._id_program_cylinder_side == 0) {
    loadCylinderSideShader();
  }
  if (_id_prog._id_program_oriented_circle == 0) {
    loadOrientedCircleShader();
  }
  _world.addObject(new Cylinder(positions, radius, colors,
                                _id_prog._id_program_cylinder_side,
                                _id_prog._id_program_oriented_circle));
}

/**
 * add capsule to visualizer
 * @param positions vector of positions of length 6*num, for each arrow:
 *        from_x, from_y, from_z, to_x, to_y, to_z
 * @param radius
 * @param colors
 */
void CommonVisualizer::addCapsule(const std::vector<float> &positions,
                                  const std::vector<float> &radius,
                                  const std::vector<Vec> &colors) {
  if (_id_prog._id_program_cylinder_side == 0) {
    loadCylinderSideShader();
  }
  if (_id_prog._id_program_unicolor_cloud_sphere == 0) {
    loadUnicolorCloudSphereShader();
  }
  _world.addObject(new Capsule(positions, radius, colors,
                               _id_prog._id_program_cylinder_side,
                               _id_prog._id_program_unicolor_cloud_sphere));
}

/**
 * add cone to visualizer
 * @param positions vector of positions of length 6*num, for each arrow:
 *        from_x, from_y, from_z, to_x, to_y, to_z
 * @param radius
 * @param colors
 */
void CommonVisualizer::addCone(const std::vector<float> &positions,
                               const std::vector<float> &radius,
                               const std::vector<Vec> &colors) {
  if (_id_prog._id_program_cone_side == 0) {
    loadConeSideShader();
  }
  if (_id_prog._id_program_oriented_circle == 0) {
    loadOrientedCircleShader();
  }
  _world.addObject(new Cone(positions, radius, colors,
                            _id_prog._id_program_cone_side,
                            _id_prog._id_program_oriented_circle));
}

/**
 * add arrows to cisualizer
 * @param positions vector of positions of length 6*num, for each arrow:
 *        from_x, from_y, from_z, to_x, to_y, to_z
 * @param length_head
 * @param radius_head
 * @param radius_body
 * @param colors
 */
void CommonVisualizer::addArrow(const std::vector<float> &positions,
                                const std::vector<float> &length_head,
                                const std::vector<float> &radius_head,
                                const std::vector<float> &radius_body,
                                const std::vector<Vec> &colors) {
  if (_id_prog._id_program_cylinder_side == 0) {
    loadCylinderSideShader();
  }
  if (_id_prog._id_program_cone_side == 0) {
    loadConeSideShader();
  }
  if (_id_prog._id_program_oriented_circle == 0) {
    loadOrientedCircleShader();
  }
  _world.addObject(new Arrow(positions, radius_head, radius_body, length_head,
                             colors, _id_prog._id_program_cylinder_side,
                             _id_prog._id_program_cone_side,
                             _id_prog._id_program_oriented_circle));
}

/**
 * add box with transform, scaling and one color
 * @param transform
 * @param scale
 * @param color
 */
void CommonVisualizer::addBox(const Transform &transform, const Vec &scale,
                              const Vec &color) {
  if (_id_prog._id_program_unicolor_mesh == 0) {
    loadUnicolorMeshShader();
  }
  _world.addObject(
      new Box(transform, scale, color, _id_prog._id_program_unicolor_mesh));
}

/**
 * load shader for mesh rendering from source
 */
void CommonVisualizer::loadColoredMeshShader() {
  const char *vs =
#include "./shaders/vs_colored_mesh.vs"
      ;
  const char *fs_phong =
#include "./shaders/fs_sub_phong.fs"
      ;
  const char *fs =
#include "./shaders/fs_phong_simple.fs"
      ;
  const char *texts[] = {vs, fs_phong, fs};
  GLenum types[] = {GL_VERTEX_SHADER, GL_FRAGMENT_SHADER, GL_FRAGMENT_SHADER};
  _id_prog._id_program_colored_mesh = CommonVisualizer::createProgram(
      std::vector<GLenum>(types, types + 3),
      std::vector<const char *>(texts, texts + 3));
}

/**
 * load shader for mesh with color rendering from source
 */
void CommonVisualizer::loadUnicolorMeshShader() {
  const char *vs =
#include "./shaders/vs_unicolor_mesh.vs"
      ;
  const char *fs_phong =
#include "./shaders/fs_sub_phong.fs"
      ;
  const char *fs =
#include "./shaders/fs_phong_simple.fs"
      ;
  const char *texts[] = {vs, fs_phong, fs};
  GLenum types[] = {GL_VERTEX_SHADER, GL_FRAGMENT_SHADER, GL_FRAGMENT_SHADER};
  _id_prog._id_program_unicolor_mesh = CommonVisualizer::createProgram(
      std::vector<GLenum>(types, types + 3),
      std::vector<const char *>(texts, texts + 3));
}

/**
 * load shader for colored point cloud rendering from source
 */
void CommonVisualizer::loadColoredCloudPointShader() {
  const char *vs =
#include "./shaders/vs_colored_cloud_point.vs"
      ;
  const char *fs =
#include "./shaders/fs_simple.fs"
      ;
  const char *texts[] = {vs, fs};
  GLenum types[] = {GL_VERTEX_SHADER, GL_FRAGMENT_SHADER};
  _id_prog._id_program_colored_cloud_point = CommonVisualizer::createProgram(
      std::vector<GLenum>(types, types + 2),
      std::vector<const char *>(texts, texts + 2));
}

/**
 * load shader for point cloud with unified color from source
 */
void CommonVisualizer::loadUnicolorCloudPointShader() {
  const char *vs =
#include "./shaders/vs_unicolor_cloud_point.vs"
      ;
  const char *fs =
#include "./shaders/fs_simple.fs"
      ;
  const char *texts[] = {vs, fs};
  GLenum types[] = {GL_VERTEX_SHADER, GL_FRAGMENT_SHADER};
  _id_prog._id_program_unicolor_cloud_point = CommonVisualizer::createProgram(
      std::vector<GLenum>(types, types + 2),
      std::vector<const char *>(texts, texts + 2));
}

/**
 * load shader for spheres, which has its own color, from source
 */
void CommonVisualizer::loadColoredCloudSphereShader() {
  const char *gs =
#include "./shaders/gs_cloud_sphere.gs"
      ;
  const char *vs =
#include "./shaders/vs_colored_cloud_sphere.vs"
      ;
  const char *fs =
#include "./shaders/fs_sphere.fs"
      ;
  const char *fs_phong =
#include "./shaders/fs_sub_phong.fs"
      ;
  const char *texts[] = {gs, vs, fs, fs_phong};
  GLenum types[] = {GL_GEOMETRY_SHADER, GL_VERTEX_SHADER, GL_FRAGMENT_SHADER,
                    GL_FRAGMENT_SHADER};
  _id_prog._id_program_colored_cloud_sphere = CommonVisualizer::createProgram(
      std::vector<GLenum>(types, types + 4),
      std::vector<const char *>(texts, texts + 4));
}

/**
 * load shader for spheres with same color
 */
void CommonVisualizer::loadUnicolorCloudSphereShader() {
  const char *gs =
#include "./shaders/gs_cloud_sphere.gs"
      ;
  const char *vs =
#include "./shaders/vs_unicolor_cloud_sphere.vs"
      ;
  const char *fs =
#include "./shaders/fs_sphere.fs"
      ;
  const char *fs_phong =
#include "./shaders/fs_sub_phong.fs"
      ;
  const char *texts[] = {gs, vs, fs, fs_phong};
  GLenum types[] = {GL_GEOMETRY_SHADER, GL_VERTEX_SHADER, GL_FRAGMENT_SHADER,
                    GL_FRAGMENT_SHADER};
  _id_prog._id_program_unicolor_cloud_sphere = CommonVisualizer::createProgram(
      std::vector<GLenum>(types, types + 4),
      std::vector<const char *>(texts, texts + 4));
}

/**
 * load shader for side of cylinder
 */
void CommonVisualizer::loadCylinderSideShader() {
  const char *vs =
#include "./shaders/vs_cylinder_side.vs"
      ;
  const char *fs =
#include "./shaders/fs_cylinder_side.fs"
      ;
  const char *fs_phong =
#include "./shaders/fs_sub_phong.fs"
      ;
  const char *gs =
#include "./shaders/gs_cylinder_side.gs"
      ;
  const char *texts[] = {vs, fs, fs_phong, gs};
  GLenum types[] = {GL_VERTEX_SHADER, GL_FRAGMENT_SHADER, GL_FRAGMENT_SHADER,
                    GL_GEOMETRY_SHADER};
  _id_prog._id_program_cylinder_side = CommonVisualizer::createProgram(
      std::vector<GLenum>(types, types + 4),
      std::vector<const char *>(texts, texts + 4));
}

/**
 * load shader for circle rendering
 */
void CommonVisualizer::loadOrientedCircleShader() {
  const char *vs =
#include "./shaders/vs_oriented_circle.vs"
      ;
  const char *fs =
#include "./shaders/fs_oriented_circle.fs"
      ;
  const char *fs_phong =
#include "./shaders/fs_sub_phong.fs"
      ;
  const char *gs =
#include "./shaders/gs_oriented_circle.gs"
      ;
  const char *texts[] = {vs, fs, fs_phong, gs};
  GLenum types[] = {GL_VERTEX_SHADER, GL_FRAGMENT_SHADER, GL_FRAGMENT_SHADER,
                    GL_GEOMETRY_SHADER};
  _id_prog._id_program_oriented_circle = CommonVisualizer::createProgram(
      std::vector<GLenum>(types, types + 4),
      std::vector<const char *>(texts, texts + 4));
}

/**
 * load shader for side of cones
 */
void CommonVisualizer::loadConeSideShader() {
  const char *vs =
#include "./shaders/vs_cylinder_side.vs"
      ;
  const char *fs =
#include "./shaders/fs_cone_side.fs"
      ;
  const char *fs_phong =
#include "./shaders/fs_sub_phong.fs"
      ;
  const char *gs =
#include "./shaders/gs_cylinder_side.gs"
      ;
  const char *texts[] = {vs, fs, fs_phong, gs};
  GLenum types[] = {GL_VERTEX_SHADER, GL_FRAGMENT_SHADER, GL_FRAGMENT_SHADER,
                    GL_GEOMETRY_SHADER};
  _id_prog._id_program_cone_side = CommonVisualizer::createProgram(
      std::vector<GLenum>(types, types + 4),
      std::vector<const char *>(texts, texts + 4));
}
}  // namespace loco

#endif  // COMMONVISUALIZER_COMMONVISUALIZER_HPP

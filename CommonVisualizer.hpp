//
// Created by tlou on 20.01.17.
//

#ifndef COMMONVISUALIZER_COMMONVISUALIZER_HPP
#define COMMONVISUALIZER_COMMONVISUALIZER_HPP

#include <cmath>
#include <iostream>
#include <vector>
#include <list>
#include <string>
#include <functional>

#include "include/include_gl.h"
#include "include/Vec.h"
#include "include/Container.hpp"
#include "include/MeshColored.hpp"
#include "include/MeshUnicolor.hpp"
#include "include/CloudSphereColored.hpp"
#include "include/CloudPointColored.hpp"
#include "include/CloudSphereUnicolor.hpp"
#include "include/CloudPointUnicolor.hpp"
#include "include/CoordinateUnits.hpp"
#include "include/Box.hpp"
#include "include/Capsule.hpp"
#include "include/Cylinder.hpp"

namespace loco
{
  /**
   * constants for some useful colors
   */
  namespace color
  {
    const Vec BLACK{0.0f, 0.0f, 0.0f, 1.0f};
    const Vec GREY{0.5f, 0.5f, 0.5f, 1.0f};
    const Vec WHITE{1.0f, 1.0f, 1.0f, 1.0f};
    const Vec RED{1.0f, 0.0f, 0.0f, 1.0f};
    const Vec GREEN{0.0f, 1.0f, 0.0f, 1.0f};
    const Vec BLUE{0.0f, 0.0f, 1.0f, 1.0f};
    const Vec YELLOW{1.0f, 1.0f, 0.0f, 1.0f};
    const Vec MAGENTA{1.0f, 0.0f, 1.0f, 1.0f};
    const Vec CYAN{0.0f, 1.0f, 1.0f, 1.0f};
  }

  union GlProgram
  {
    GLuint _ids[20];
    struct
    {
      GLuint _id_program_colored_mesh;
      GLuint _id_program_unicolor_mesh;
      GLuint _id_program_colored_cloud_point;
      GLuint _id_program_unicolor_cloud_point;
      GLuint _id_program_colored_cloud_sphere;
      GLuint _id_program_unicolor_cloud_sphere;
      GLuint _id_program_cylinder_side;
      GLuint _id_program_oriented_circle;
    };
  };

//  // callback for resizing window, may remove because maybe already included
//  std::function<void(int, int)> callback_resize_window;
//  extern "C" void wrapper_resize_window(int i, int j)
//  {
//    callback_resize_window(i, j);
//  }

  class CommonVisualizer
  {
  protected:
    int _height, _width;
    GLFWwindow *_window;
    bool _re_init;
    float _distance, _theta, _phi;
    glm::vec4 _pos_viewer;
    glm::vec4 _up_viewer;
    glm::vec4 _center_viewer;
    LightSource _light_source;
    Vec _param_phong;
    glm::mat4 _mat_proj;
    glm::mat4 _mat_view;
    glm::mat4 _mat_external;
    GlProgram _id_prog;
    Container _world;

    /**
     * print error by compilation
     * @param length
     * @param id
     */
    static void dispErrorCompilation(const int length, const GLuint id, const GLenum &type)
    {
      std::string error;
      error.resize(length + 1);
      switch(type)
      {
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
     * create single shader with source code shader_text and shader type(vertex, fragement...)
     * @param shader_type
     * @param shader_text
     * @return
     */
    static GLuint createShader(const GLenum &shader_type, const char *shader_text)
    {
      GLuint id_shader = glCreateShader(shader_type);
      GLint re = GL_FALSE;
      int len_info;
      glShaderSource(id_shader, 1, &shader_text, NULL);
      glCompileShader(id_shader);
      glGetShaderiv(id_shader, GL_COMPILE_STATUS, &re);
      glGetShaderiv(id_shader, GL_INFO_LOG_LENGTH, &len_info);
      if(len_info > 0)
      {
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
    static GLuint createProgram(const std::vector<GLenum> &shader_types,
                                const std::vector<const char *> &shader_texts)
    {
      assert(shader_types.size() == shader_texts.size());
      GLuint id_program;
      GLint re = GL_FALSE;
      int len_info;
      std::vector<GLuint> id_shaders;
      id_shaders.reserve(shader_texts.size());

      // create each shader
      for(size_t id = 0; id < shader_texts.size(); ++id)
      {
        id_shaders.push_back(CommonVisualizer::createShader(shader_types.at(id), shader_texts.at(id)));
      }
      // attach to program
      id_program = glCreateProgram();
      for(GLuint &id_shader : id_shaders)
      {
        glAttachShader(id_program, id_shader);
      }
      glLinkProgram(id_program);
      glGetProgramiv(id_program, GL_LINK_STATUS, &re);
      glGetProgramiv(id_program, GL_INFO_LOG_LENGTH, &len_info);
      if(len_info > 0)
      {
        std::cout << "error by program" << std::endl;
        dispErrorCompilation(len_info, id_program, GL_PROGRAM);
      }
      // detach and release shaders
      for(GLuint &id_shader : id_shaders)
      {
        glDetachShader(id_program, id_shader);
        glDeleteShader(id_shader);
      }
      return id_program;
    }

    /**
     * update position of viewer (eye) with external transform
     */
    void updatePosViewer()
    {
      _pos_viewer = _mat_external * glm::vec4(_distance * (float) sin(_theta) * (float) sin(_phi),
                                              _distance * (float) sin(_theta) * (float) cos(_phi),
                                              _distance * (float) cos(_theta),
                                              1.0f);
    }

    /**
     * update direction of up with external transform
     */
    void updateUpViewer()
    {
      double theta_ = (double) _theta - M_PI / 2.0;
      double phi_ = -(double) _phi;
      _up_viewer = _mat_external * glm::vec4((float) sin(theta_) * (float) sin(phi_),
                                             (float) sin(theta_) * (float) cos(phi_),
                                             (float) cos(theta_),
                                             0.0f);
    }

    /**
     * update center (where to look at) with external transform
     */
    void updateCenterViewer()
    {
      _center_viewer = _mat_external * glm::vec4(0.0f, 0.0f, 0.0f, 1.0f);
    }

    /**
     * update viewing matrix with position, look at position and up direction of viewer (class member)
     */
    void updateViewingMatrix()
    {
      updatePosViewer();
      updateCenterViewer();
      updateUpViewer();
      _mat_view = glm::lookAt(glm::vec3(_pos_viewer.x, _pos_viewer.y, _pos_viewer.z),
                              glm::vec3(_center_viewer.x, _center_viewer.y, _center_viewer.z),
                              glm::vec3(_up_viewer.x, _up_viewer.y, _up_viewer.z));
    }

    /**
     * rotate and zoom(not implemented) according to mouse input
     */
    void onRotationAndZooming()
    {
//      const int BUTTON_ZOOM = GLFW_MOUSE_BUTTON_RIGHT; // in case middle button doesn't work
      const int BUTTON_ZOOM = GLFW_MOUSE_BUTTON_MIDDLE;
      static double xpos_prev = 0.0, ypos_prev = 0.0;
      static bool is_left_pressed = false;
      static bool is_middle_pressed = false;
      if(!is_left_pressed && glfwGetMouseButton(_window, GLFW_MOUSE_BUTTON_LEFT) == GLFW_PRESS)
      {
        glfwGetCursorPos(_window, &xpos_prev, &ypos_prev);
        is_left_pressed = true;
      }
      else if(is_left_pressed && glfwGetMouseButton(_window, GLFW_MOUSE_BUTTON_LEFT) == GLFW_RELEASE)
      {
        double xpos = 0.0, ypos = 0.0;
        glfwGetCursorPos(_window, &xpos, &ypos);
        double dx = xpos - xpos_prev;
        double dy = ypos - ypos_prev;
        if(fabs(dx) > 10.0 * fabs(dy))
        {
          dy = 0.0;
        }
        else if(fabs(dx) * 10.0 < fabs(dy))
        {
          dx = 0.0;
        }
        _theta -= dy * M_PI * 0.25 / (double) _height;
        _phi += dx * M_PI * 0.25 / (double) _width;
        if(_theta < 0.0)
        {
          _theta = 0.0;
        }
        if(_theta > M_PI)
        {
          _theta = M_PI;
        }
        updateViewingMatrix();
        is_left_pressed = false;
      }
      else if(!is_middle_pressed && glfwGetMouseButton(_window, BUTTON_ZOOM) == GLFW_PRESS)
      {
        glfwGetCursorPos(_window, &xpos_prev, &ypos_prev);
        is_middle_pressed = true;
      }
      else if(is_middle_pressed && glfwGetMouseButton(_window, BUTTON_ZOOM) == GLFW_RELEASE)
      {
        double xpos = 0.0, ypos = 0.0;
        glfwGetCursorPos(_window, &xpos, &ypos);
        double dy = ypos - ypos_prev;
        _distance *= (float) pow(1.1, dy / 50.0);
        updateViewingMatrix();
        is_middle_pressed = false;
      }
    }

//    /**
//     * function called by callback function for resizing window
//     *          may remove because maybe already included
//     * @param width
//     * @param height
//     */
//    void resizeWindow(int width, int height)
//    {
//      _width = width;
//      _height = height;
//      _mat_proj = glm::perspective(glm::radians(45.0f), (float) _width / (float) _height,
//                                   0.1f, 500.0f);
//    }

    /**
     * update viewer pose in program
     * @param id_program
     */
    void updateViewerPose(const GLuint &id_program)
    {
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
    void updatePhongParameter(const GLuint &id_program)
    {
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
    void updatePhongParameter()
    {
      for(int i = 0; i < 20; ++i)
      {
        if(_id_prog._ids[i])
        {
          updatePhongParameter(_id_prog._ids[i]);
        }
      }
      if(_id_prog._id_program_unicolor_cloud_sphere)
      {
        updateViewerPose(_id_prog._id_program_unicolor_cloud_sphere);
      }
      if(_id_prog._id_program_colored_cloud_sphere)
      {
        updateViewerPose(_id_prog._id_program_colored_cloud_sphere);
      }
      if(_id_prog._id_program_cylinder_side)
      {
        updateViewerPose(_id_prog._id_program_cylinder_side);
      }
      if(_id_prog._id_program_oriented_circle)
      {
        updateViewerPose(_id_prog._id_program_oriented_circle);
      }
    }

  public:
    CommonVisualizer(const int height = 600, const int width = 800, const char *name = "default",
                     const Vec &background = color::BLACK) :
        _height(height),
        _width(width),
        _re_init(0),
        _window(NULL),
        _distance(3.0f),
        _theta(M_PI / 2.0),
        _phi(0.0f),
        _mat_proj(glm::perspective(glm::radians(45.0f), (float) _width / (float) _height, 0.1f, 5000.0f)),
        _mat_external(Object::transformToMat4(Transform{Vec{}, Vec{0.0f, 0.0f, 0.0f, 1.0f}})),
        _param_phong(Vec{0.3f, 0.3f, 0.4f, 2.0f})
    {
      // glfw init
      _re_init = (glfwInit() == GLFW_TRUE);
      if(!_re_init)
      {
        return;
      }
      glfwWindowHint(GLFW_SAMPLES, 4);
      glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
      glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
      glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
      glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

      // window init
      _window = glfwCreateWindow(_width, _height, name, NULL, NULL);
      if(!_window)
      {
        return;
      }
      glfwMakeContextCurrent(_window);

      // glew init
      glewExperimental = GL_TRUE;
      _re_init = (glewInit() == GLEW_OK); // before is true, same as and
      if(!_re_init)
      {
        return;
      }

      glfwSetInputMode(_window, GLFW_STICKY_MOUSE_BUTTONS, GL_TRUE);
//      glfwSetInputMode(_window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
      glfwSetInputMode(_window, GLFW_STICKY_KEYS, GL_TRUE);

      glClearColor(background._r, background._g, background._b, background._a);
      glEnable(GL_DEPTH_TEST);
      glDepthFunc(GL_LESS);
      glEnable(GL_CULL_FACE); // backface culling
      glEnable(GL_PROGRAM_POINT_SIZE);
      glEnable(GL_BLEND);
      glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

//      // resize windows callback, may be already inclueded in some lib
//      int whatever = 0;
//      glutInit(&whatever, (char**)NULL);
//      callback_resize_window = std::bind(&CommonVisualizer::resizeWindow, this,
//                                         std::placeholders::_1, std::placeholders::_2);
//      glutReshapeFunc(wrapper_resize_window);

      for(int i = 0; i < 20; ++i)
      {
        _id_prog._ids[i] = 0;
      }

      updateViewingMatrix();
    }

    ~CommonVisualizer()
    {
      for(int i = 0; i < 20; ++i)
      {
        if(_id_prog._ids[i])
        {
          glDeleteProgram(_id_prog._ids[i]);
        }
      }
      glfwTerminate();
    }

    /**
     * clear all objects
     */
    void resetScene()
    {
      _world.reset();
    }

    /**
     * set distance from eye to center (zooming)
     * @param distance
     */
    void setDistance(const float distance)
    {
      _distance = distance;
      updateViewingMatrix();
    }

    /**
     * set theta of viewer, which is the angle of viewing direction to -Z
     * @param theta
     */
    void setTheta(const float theta)
    {
      _theta = theta;
      updateViewingMatrix();
    }

    /**
     * set phi of view, which is rotation around Z-axis
     * @param phi
     */
    void setPhi(const float phi)
    {
      _phi = phi;
      updateViewingMatrix();
    }

    /**
     * displace once
     * @return whether to continue
     */
    bool playOnce()
    {
//      glfwSetTime(0.0);

      glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
      onRotationAndZooming();
      glm::mat4 proj = _mat_proj * _mat_view; // without its own tf

      updatePhongParameter();
      _world.display(proj);

      glfwSwapBuffers(_window);
      glfwPollEvents();

//      double end_time = glfwGetTime();
//      std::cout << "fps: " << 1.0 / end_time << std::endl;

      return glfwGetKey(_window, GLFW_KEY_ESCAPE) != GLFW_PRESS
             && glfwWindowShouldClose(_window) == 0;
    }

    /**
     * render in loop
     */
    void play()
    {
      while(playOnce())
      {
      }
    }

    /**
     * checks intialiazation
     * @return whether the initialization is correct
     */
    bool isInited() const
    {
      return _re_init && _window != NULL;
    }

    /**
     * add point light source (other are not necessary, point source shows the shape well)
     * @param pos
     * @param color
     */
    void setLightSource(const Vec &pos, const Vec &color)
    {
      _light_source._pos = pos;
      _light_source._color = color;
    }

    /**
     * move camera with translation and rotation
     * @param transform
     */
    void setTransform(const Transform &transform)
    {
      _mat_external = Object::transformToMat4(transform);
      updatePosViewer();
      updateUpViewer();
      updateCenterViewer();
      updateViewingMatrix();
    }

    void setTransform(const int id, const Transform &transform)
    {
      _world.setTransform(id, transform);
    }

    /**
     * add mesh with uniform color
     * @param vertices list of vertex, every 9 floats is a trangle
     * @param color uniform color for this mesh
     */
    void addMesh(const std::vector<float> &vertices, const Vec &color)
    {
      if(_id_prog._id_program_unicolor_mesh == 0)
      {
        loadUnicolorMeshShader();
      }
      _world.addObject(new MeshUnicolor(vertices, color, _id_prog._id_program_unicolor_mesh));
    }

    void addMesh(const std::vector<float> &vertices, const std::vector<int> &index, const Vec &color)
    {
      if(_id_prog._id_program_unicolor_mesh == 0)
      {
        loadUnicolorMeshShader();
      }
      _world.addObject(new MeshUnicolor(vertices, index, color, _id_prog._id_program_unicolor_mesh));
    }

    /**
     * add mesh with color
     * @param vertices list of vertex, every 9 floats is a trangle
     * @param colors list of color value, every 4 floats color on a vertex
     */
    void addMesh(const std::vector<float> &vertices, const std::vector<float> &colors)
    {
      if(_id_prog._id_program_colored_mesh == 0)
      {
        loadColoredMeshShader();
      }
      _world.addObject(new MeshColored(vertices, colors, _id_prog._id_program_colored_mesh));
    }

    /**
     *
     * @param points
     * @param colors
     * @param radius
     */
    void addPointCloud(const std::vector<float> &points, const std::vector<float> &colors,
                       const float radius = -1.0f)
    {
      if(radius <= 0.0f)
      {
        if(_id_prog._id_program_colored_cloud_point == 0)
        {
          loadColoredCloudPointShader();
        }
        _world.addObject(new CloudPointColored(points, colors, -radius,
                                               _id_prog._id_program_colored_cloud_point));
      }
      else
      {
        if(_id_prog._id_program_colored_cloud_sphere == 0)
        {
          loadColoredCloudSphereShader();
        }
        _world.addObject(new CloudSphereColored(points, colors, radius,
                                                _id_prog._id_program_colored_cloud_sphere));
      }
    }

    void addPointCloud(const std::vector<float> &points, const Vec &color,
                       const float radius = -1.0f)
    {
      if(radius <= 0.0f)
      {
        if(_id_prog._id_program_unicolor_cloud_point == 0)
        {
          loadUnicolorCloudPointShader();
        }
        _world.addObject(new CloudPointUnicolor(points, color, -radius,
                                                _id_prog._id_program_unicolor_cloud_point));
      }
      else
      {
        if(_id_prog._id_program_unicolor_cloud_sphere == 0)
        {
          loadUnicolorCloudSphereShader();
        }
        _world.addObject(new CloudSphereUnicolor(points, color, radius,
                                                 _id_prog._id_program_unicolor_cloud_sphere));
      }
    }

    /**
     * add the notation for coordinate system after transform
     * @param transform
     */
    void addCoordinateSign(const Transform &transform)
    {
      if(_id_prog._id_program_colored_cloud_point == 0)
      {
        loadColoredCloudPointShader();
      }
      _world.addObject(new CoordinateUnits(_id_prog._id_program_colored_cloud_point));
    }

    void addCylinder(const std::vector<float> &positions, const std::vector<float> &radius,
                     const std::vector<Vec> &colors)
    {
      if(_id_prog._id_program_cylinder_side == 0)
      {
        loadCylinderSideShader();
      }
      if(_id_prog._id_program_oriented_circle == 0)
      {
        loadOrientedCircleShader();
      }
      _world.addObject(new Cylinder(positions, radius, colors,
                                    _id_prog._id_program_cylinder_side,
                                    _id_prog._id_program_oriented_circle));
    }

    void addCapsule(const std::vector<float> &positions, const std::vector<float> &radius,
                    const std::vector<Vec> &colors)
    {
      if(_id_prog._id_program_cylinder_side == 0)
      {
        loadCylinderSideShader();
      }
      if(_id_prog._id_program_unicolor_cloud_sphere == 0)
      {
        loadUnicolorCloudSphereShader();
      }
      _world.addObject(new Capsule(positions, radius, colors,
                                   _id_prog._id_program_cylinder_side,
                                   _id_prog._id_program_unicolor_cloud_sphere));
    }

    /**
     * add box with transform, scaling and one color
     * @param transform
     * @param scale
     * @param color
     */
    void addBox(const Transform &transform, const Vec &scale, const Vec &color)
    {
      if(_id_prog._id_program_unicolor_mesh == 0)
      {
        loadUnicolorMeshShader();
      }
      _world.addObject(new Box(transform, scale, color, _id_prog._id_program_unicolor_mesh));
    }

    void loadColoredMeshShader()
    {
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
          std::vector<GLenum>(types, types + 3), std::vector<const char *>(texts, texts + 3));
    }

    void loadUnicolorMeshShader()
    {
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
          std::vector<GLenum>(types, types + 3), std::vector<const char *>(texts, texts + 3));
    }

    void loadColoredCloudPointShader()
    {
      const char *vs =
#include "./shaders/vs_colored_cloud_point.vs"
      ;
      const char *fs =
#include "./shaders/fs_simple.fs"
      ;
      const char *texts[] = {vs, fs};
      GLenum types[] = {GL_VERTEX_SHADER, GL_FRAGMENT_SHADER};
      _id_prog._id_program_colored_cloud_point = CommonVisualizer::createProgram(
          std::vector<GLenum>(types, types + 2), std::vector<const char *>(texts, texts + 2));
    }

    void loadUnicolorCloudPointShader()
    {
      const char *vs =
#include "./shaders/vs_unicolor_cloud_point.vs"
      ;
      const char *fs =
#include "./shaders/fs_simple.fs"
      ;
      const char *texts[] = {vs, fs};
      GLenum types[] = {GL_VERTEX_SHADER, GL_FRAGMENT_SHADER};
      _id_prog._id_program_unicolor_cloud_point = CommonVisualizer::createProgram(
          std::vector<GLenum>(types, types + 2), std::vector<const char *>(texts, texts + 2));
    }

    void loadColoredCloudSphereShader()
    {
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
      GLenum types[] = {GL_GEOMETRY_SHADER, GL_VERTEX_SHADER, GL_FRAGMENT_SHADER, GL_FRAGMENT_SHADER};
      _id_prog._id_program_colored_cloud_sphere = CommonVisualizer::createProgram(
          std::vector<GLenum>(types, types + 4), std::vector<const char *>(texts, texts + 4));
    }

    void loadUnicolorCloudSphereShader()
    {
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
      GLenum types[] = {GL_GEOMETRY_SHADER, GL_VERTEX_SHADER, GL_FRAGMENT_SHADER, GL_FRAGMENT_SHADER};
      _id_prog._id_program_unicolor_cloud_sphere = CommonVisualizer::createProgram(
          std::vector<GLenum>(types, types + 4), std::vector<const char *>(texts, texts + 4));
    }

    void loadCylinderSideShader()
    {
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
      GLenum types[] = {GL_VERTEX_SHADER, GL_FRAGMENT_SHADER, GL_FRAGMENT_SHADER, GL_GEOMETRY_SHADER};
      _id_prog._id_program_cylinder_side = CommonVisualizer::createProgram(
          std::vector<GLenum>(types, types + 4), std::vector<const char *>(texts, texts + 4));
    }

    void loadOrientedCircleShader()
    {
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
      GLenum types[] = {GL_VERTEX_SHADER, GL_FRAGMENT_SHADER, GL_FRAGMENT_SHADER, GL_GEOMETRY_SHADER};
      _id_prog._id_program_oriented_circle = CommonVisualizer::createProgram(
          std::vector<GLenum>(types, types + 4), std::vector<const char *>(texts, texts + 4));
    }
  };
}

#endif //COMMONVISUALIZER_COMMONVISUALIZER_HPP

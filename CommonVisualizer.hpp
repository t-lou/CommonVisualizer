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

#include "include_gl.h"

namespace loco
{
  /**
   * length 4 vector (can be used as vec3)
   */
  union Vec
  {
    float _data[4];
    struct
    {
      float _x;
      float _y;
      float _z;
      float _w;
    };
    struct
    {
      float _r;
      float _g;
      float _b;
      float _a;
    };
  };

  struct Transform
  {
    Vec _translation;
    Vec _rotation;
  };

  struct LightSource
  {
    Vec _pos;
    Vec _color;
  };

  /**
   * data for colored and unicolor mesh
   */
  struct MeshBuffer
  {
    GLuint _buffer_vertex;
    GLuint _buffer_color;
    GLuint _buffer_normal;
    Vec _color;
    GLuint _id_array;
    int _size;
    glm::mat4 _transform_local;
  };

  struct PointCloudBuffer
  {
    GLuint _buffer_pos;
    GLuint _buffer_color;
    Vec _color;
    GLuint _id_array;
    int _size;
    float _radius;
    glm::mat4 _transform_local;
  };

  /**
   * constants for some useful colors
   */
  namespace color
  {
    const Vec BLACK    {0.0f, 0.0f, 0.0f, 1.0f};
    const Vec GREY     {0.5f, 0.5f, 0.5f, 1.0f};
    const Vec WHITE    {1.0f, 1.0f, 1.0f, 1.0f};
    const Vec RED      {1.0f, 0.0f, 0.0f, 1.0f};
    const Vec GREEN    {0.0f, 1.0f, 0.0f, 1.0f};
    const Vec BLUE     {0.0f, 0.0f, 1.0f, 1.0f};
    const Vec YELLOW   {1.0f, 1.0f, 0.0f, 1.0f};
    const Vec MAGENTA  {1.0f, 0.0f, 1.0f, 1.0f};
    const Vec CYAN     {0.0f, 1.0f, 1.0f, 1.0f};
  }

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
    std::list<MeshBuffer> _mesh_list;
    std::list<PointCloudBuffer> _cloud_list;
    std::list<PointCloudBuffer> _coordinate_sign_list;
    float _distance, _theta, _phi;
    glm::vec4 _pos_viewer;
    glm::vec4 _up_viewer;
    glm::vec4 _center_viewer;
    LightSource _light_source;
    Vec _param_phong;
    glm::mat4 _mat_proj;
    glm::mat4 _mat_view;
    glm::mat4 _mat_external;
    GLuint _id_program_colored_mesh;
    GLuint _id_program_unicolor_mesh;
    GLuint _id_program_colored_cloud_point;
    GLuint _id_program_unicolor_cloud_point;
    GLuint _id_program_colored_cloud_sphere;
    GLuint _id_program_unicolor_cloud_sphere;

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
    static GLuint createShader(const GLenum &shader_type, const char* shader_text)
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
        std::cout << "error by shader" << std::endl;
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
                                const std::vector<const char*> &shader_texts)
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
      _pos_viewer = _mat_external * glm::vec4(_distance * (float)sin(_theta) * (float)sin(_phi),
                                              _distance * (float)sin(_theta) * (float)cos(_phi),
                                              _distance * (float)cos(_theta),
                                              1.0f);
    }

    /**
     * update direction of up with external transform
     */
    void updateUpViewer()
    {
      double theta_ = (double)_theta - M_PI / 2.0;
      double phi_ = -(double)_phi;
      _up_viewer = _mat_external * glm::vec4((float)sin(theta_) * (float)sin(phi_),
                                             (float)sin(theta_) * (float)cos(phi_),
                                             (float)cos(theta_),
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

    static void print(const glm::vec4 &vec)
    {
      for(int j = 0; j < 4; ++j)
      {
        std::cout << vec[j] << " ";
      }
      std::cout << std::endl;
    }

    static void print(const glm::mat4 &mat)
    {
      for(int i = 0; i < 4; ++i)
      {
        for(int j = 0; j < 4; ++j)
        {
          std::cout << mat[i][j] << " ";
        }
        std::cout << std::endl;
      }
      std::cout << std::endl;
    }

    /**
     * get transform matrix from translation and rotation quaternion
     * @param transform
     * @return
     */
    static glm::mat4 transformToMat4(const Transform &transform)
    {
      glm::mat4 tf = glm::mat4_cast(glm::quat(transform._rotation._w, transform._rotation._x,
                                              transform._rotation._y, transform._rotation._z));
//      glm::mat4 tf = glm::mat4_cast(glm::quat(transform._rotation._x, transform._rotation._y,
//                                              transform._rotation._z, transform._rotation._w));
      tf[3] = glm::vec4(transform._translation._x, transform._translation._y, transform._translation._z, 1.0f);
      return tf;
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
        _theta += dy * M_PI * 0.25 / (double)_height;
        _phi -= dx * M_PI * 0.25 / (double)_width;
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
        _distance *= (float)pow(1.1, dy / 50.0);
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
     * prepare parameters for phong lighting to program
     * @param id_program
     */
    void preparePhongParameter(const GLuint &id_program)
    {
      GLuint id_pos_source = glGetUniformLocation(id_program, "pos_light");
      GLuint id_color_source = glGetUniformLocation(id_program, "color_light");
      GLuint id_pos_viewer = glGetUniformLocation(id_program, "pos_viewer");
      GLuint id_param_phong = glGetUniformLocation(id_program, "phong");
      glUniform3fv(id_pos_source, 1, _light_source._pos._data);
      glUniform4fv(id_color_source, 1, _light_source._color._data);
      glUniform3fv(id_pos_viewer, 1, &_pos_viewer[0]);
      glUniform4fv(id_param_phong, 1, _param_phong._data);
    }

    /**
     * render mesh with uniform color from _mesh_list
     * @param proj
     */
    void renderUnicolorMesh(const glm::mat4 &proj)
    {
      GLuint id_proj = glGetUniformLocation(_id_program_unicolor_mesh, "proj");
      GLuint id_color = glGetUniformLocation(_id_program_unicolor_mesh, "color");
      glUseProgram(_id_program_unicolor_mesh);
      preparePhongParameter(_id_program_unicolor_mesh);
      for(const MeshBuffer &mesh : _mesh_list)
      {
        if(!mesh._buffer_color)
        {
          glm::mat4 total_proj = proj * mesh._transform_local;
          glUniformMatrix4fv(id_proj, 1, GL_FALSE, &total_proj[0][0]);

          glUniform4fv(id_color, 1, mesh._color._data);
          glBindVertexArray(mesh._id_array);

          glEnableVertexAttribArray(0);
          glBindBuffer(GL_ARRAY_BUFFER, mesh._buffer_vertex);
          glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, NULL);

          glEnableVertexAttribArray(1);
          glBindBuffer(GL_ARRAY_BUFFER, mesh._buffer_normal);
          glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 0, NULL);

          glDrawArrays(GL_TRIANGLES, 0, mesh._size);

          glEnableVertexAttribArray(1);
          glDisableVertexAttribArray(0);
        }
      }
    }

    /**
     * render mesh with color in _mesh_list
     * @param proj
     */
    void renderColoredMesh(const glm::mat4 &proj)
    {
      GLuint id_proj = glGetUniformLocation(_id_program_colored_mesh, "proj");
      glUseProgram(_id_program_colored_mesh);
      preparePhongParameter(_id_program_colored_mesh);
      for(const MeshBuffer &mesh : _mesh_list)
      {
        if(mesh._buffer_color)
        {
          glm::mat4 total_proj = proj * mesh._transform_local;
          glUniformMatrix4fv(id_proj, 1, GL_FALSE, &total_proj[0][0]);
          glBindVertexArray(mesh._id_array);

          glEnableVertexAttribArray(0);
          glBindBuffer(GL_ARRAY_BUFFER, mesh._buffer_vertex);
          glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, NULL);

          glEnableVertexAttribArray(1);
          glBindBuffer(GL_ARRAY_BUFFER, mesh._buffer_color);
          glVertexAttribPointer(1, 4, GL_FLOAT, GL_FALSE, 0, NULL);

          glEnableVertexAttribArray(2);
          glBindBuffer(GL_ARRAY_BUFFER, mesh._buffer_normal);
          glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, 0, NULL);

          glDrawArrays(GL_TRIANGLES, 0, mesh._size);

          glDisableVertexAttribArray(2);
          glDisableVertexAttribArray(1);
          glDisableVertexAttribArray(0);
        }
      }
    }

    void renderColoredCloudPoint(const glm::mat4 &proj)
    {
      GLuint id_proj = glGetUniformLocation(_id_program_colored_cloud_point, "proj");
      GLuint id_size = glGetUniformLocation(_id_program_unicolor_cloud_point, "point_size");
      glUseProgram(_id_program_colored_cloud_point);
      for(const PointCloudBuffer &cloud : _cloud_list)
      {
        if(cloud._radius <= 0.0f && cloud._buffer_color)
        {
          float point_size = cloud._radius < -1.0f ? -cloud._radius : 2.0f;
          glm::mat4 total_proj = proj * cloud._transform_local;
          glUniformMatrix4fv(id_proj, 1, GL_FALSE, &total_proj[0][0]);
          glUniform1f(id_size, point_size);
          glBindVertexArray(cloud._id_array);

          glEnableVertexAttribArray(0);
          glBindBuffer(GL_ARRAY_BUFFER, cloud._buffer_pos);
          glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, NULL);

          glEnableVertexAttribArray(1);
          glBindBuffer(GL_ARRAY_BUFFER, cloud._buffer_color);
          glVertexAttribPointer(1, 4, GL_FLOAT, GL_FALSE, 0, NULL);

          glDrawArrays(GL_POINTS, 0, cloud._size);

          glDisableVertexAttribArray(1);
          glDisableVertexAttribArray(0);
        }
      }
    }

    void renderUnicolorCloudPoint(const glm::mat4 &proj)
    {
      GLuint id_proj = glGetUniformLocation(_id_program_unicolor_cloud_point, "proj");
      GLuint id_color = glGetUniformLocation(_id_program_unicolor_cloud_point, "color");
      GLuint id_size = glGetUniformLocation(_id_program_unicolor_cloud_point, "point_size");
      glUseProgram(_id_program_unicolor_cloud_point);
      for(const PointCloudBuffer &cloud : _cloud_list)
      {
        if(cloud._radius <= 0.0f && !cloud._buffer_color)
        {
          float point_size = cloud._radius < -1.0f ? -cloud._radius : 2.0f;
          glm::mat4 total_proj = proj * cloud._transform_local;
          glUniformMatrix4fv(id_proj, 1, GL_FALSE, &total_proj[0][0]);
          glUniform1f(id_size, point_size);
          glUniform4fv(id_color, 1, cloud._color._data);
          glBindVertexArray(cloud._id_array);

          glEnableVertexAttribArray(0);
          glBindBuffer(GL_ARRAY_BUFFER, cloud._buffer_pos);
          glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, NULL);

          glDrawArrays(GL_POINTS, 0, cloud._size);

          glDisableVertexAttribArray(0);
        }
      }
    }

    void renderColoredCloudSphere(const glm::mat4 &proj)
    {
      GLuint id_proj = glGetUniformLocation(_id_program_colored_cloud_sphere, "proj");
      GLuint id_size = glGetUniformLocation(_id_program_colored_cloud_sphere, "radius");
      GLuint id_up = glGetUniformLocation(_id_program_colored_cloud_sphere, "dir_up");
      GLuint id_eye = glGetUniformLocation(_id_program_colored_cloud_sphere, "pos_eye");
      glUseProgram(_id_program_colored_cloud_sphere);
      preparePhongParameter(_id_program_colored_cloud_sphere);
      for(const PointCloudBuffer &cloud : _cloud_list)
      {
        if(cloud._radius > 0.0f && cloud._buffer_color)
        {
          glm::mat4 total_proj = proj * cloud._transform_local;
          glUniformMatrix4fv(id_proj, 1, GL_FALSE, &total_proj[0][0]);

          glUniform1f(id_size, cloud._radius);
          glUniform3fv(id_up, 1, &_up_viewer[0]);
          glUniform3fv(id_eye, 1, &_pos_viewer[0]);
          glBindVertexArray(cloud._id_array);

          glEnableVertexAttribArray(0);
          glBindBuffer(GL_ARRAY_BUFFER, cloud._buffer_pos);
          glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, NULL);

          glEnableVertexAttribArray(1);
          glBindBuffer(GL_ARRAY_BUFFER, cloud._buffer_color);
          glVertexAttribPointer(1, 4, GL_FLOAT, GL_FALSE, 0, NULL);

          glDrawArrays(GL_POINTS, 0, cloud._size);

          glDisableVertexAttribArray(1);
          glDisableVertexAttribArray(0);
        }
      }
    }

    void renderUnicolorCloudSphere(const glm::mat4 &proj)
    {
      GLuint id_proj = glGetUniformLocation(_id_program_unicolor_cloud_sphere, "proj");
      GLuint id_color = glGetUniformLocation(_id_program_unicolor_cloud_sphere, "color");
      GLuint id_size = glGetUniformLocation(_id_program_unicolor_cloud_sphere, "radius");
      GLuint id_up = glGetUniformLocation(_id_program_unicolor_cloud_sphere, "dir_up");
      GLuint id_eye = glGetUniformLocation(_id_program_unicolor_cloud_sphere, "pos_eye");
      glUseProgram(_id_program_unicolor_cloud_sphere);
      preparePhongParameter(_id_program_unicolor_cloud_sphere);
      for(const PointCloudBuffer &cloud : _cloud_list)
      {
        if(cloud._radius > 0.0f && !cloud._buffer_color)
        {
          glm::mat4 total_proj = proj * cloud._transform_local;
          glUniformMatrix4fv(id_proj, 1, GL_FALSE, &total_proj[0][0]);

          glUniform1f(id_size, cloud._radius);
          glUniform4fv(id_color, 1, cloud._color._data);
          glUniform3fv(id_up, 1, &_up_viewer[0]);
          glUniform3fv(id_eye, 1, &_pos_viewer[0]);
          glBindVertexArray(cloud._id_array);

          glEnableVertexAttribArray(0);
          glBindBuffer(GL_ARRAY_BUFFER, cloud._buffer_pos);
          glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, NULL);

          glDrawArrays(GL_POINTS, 0, cloud._size);

          glDisableVertexAttribArray(0);
        }
      }
    }

    void renderCoordinateSign(const glm::mat4 &proj)
    {
      GLuint id_proj = glGetUniformLocation(_id_program_colored_cloud_point, "proj");
      glUseProgram(_id_program_colored_cloud_point);
      glUniformMatrix4fv(id_proj, 1, GL_FALSE, &proj[0][0]);
      for(const PointCloudBuffer &sign : _coordinate_sign_list)
      {
        glBindVertexArray(sign._id_array);

        glEnableVertexAttribArray(0);
        glBindBuffer(GL_ARRAY_BUFFER, sign._buffer_pos);
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, NULL);

        glEnableVertexAttribArray(1);
        glBindBuffer(GL_ARRAY_BUFFER, sign._buffer_color);
        glVertexAttribPointer(1, 4, GL_FLOAT, GL_FALSE, 0, NULL);

        glDrawArrays(GL_LINES, 0, sign._size);

        glDisableVertexAttribArray(1);
        glDisableVertexAttribArray(0);
      }
    }

    static std::vector<float> genFakeNormal(const std::vector<float> &vertices)
    {
      std::vector<float> normals;
      normals.resize(vertices.size());
      for(size_t id = 0; id < vertices.size(); id += 9)
      {
        // use the cross product of two edges for normal
        // vec0 <- vector from second to first
        glm::vec3 vec0(vertices.at(id + 3) - vertices.at(id + 0),
                       vertices.at(id + 4) - vertices.at(id + 1),
                       vertices.at(id + 5) - vertices.at(id + 2));
        // vec1 <- vector from second to third
        glm::vec3 vec1(vertices.at(id + 3) - vertices.at(id + 6),
                       vertices.at(id + 4) - vertices.at(id + 7),
                       vertices.at(id + 5) - vertices.at(id + 8));
        glm::vec3 nor = glm::normalize(-glm::cross(vec0, vec1));
        for(int i = 0; i < 3; ++i)
        {
          normals.at(id + i * 3 + 0) = nor[0];
          normals.at(id + i * 3 + 1) = nor[1];
          normals.at(id + i * 3 + 2) = nor[2];
        }
      }
      return normals;
    }

    static void genBufferVectorFloat(const std::vector<float>& data, GLuint &id_buffer)
    {
      glGenBuffers(1, &id_buffer);
      glBindBuffer(GL_ARRAY_BUFFER, id_buffer);
      glBufferData(GL_ARRAY_BUFFER, 4 * data.size(), &data.front(), GL_STATIC_DRAW);
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
        _mat_external(CommonVisualizer::transformToMat4(Transform{Vec{}, Vec{0.0f, 0.0f, 0.0f, 1.0f}})),
        _id_program_colored_mesh(0),
        _id_program_unicolor_mesh(0),
        _id_program_colored_cloud_point(0),
        _id_program_unicolor_cloud_point(0),
        _id_program_colored_cloud_sphere(0),
        _id_program_unicolor_cloud_sphere(0),
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
      glDepthFunc(GL_LEQUAL);
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

      updateViewingMatrix();
    }

    ~CommonVisualizer()
    {
      for(MeshBuffer &buffer : _mesh_list)
      {
        if(buffer._buffer_color)
        {
          glDeleteBuffers(1, (GLuint *) &(buffer._buffer_color));
        }
        glDeleteBuffers(1, (GLuint *) &(buffer._buffer_vertex));
        glDeleteVertexArrays(1, &buffer._id_array);
      }
      for(PointCloudBuffer &buffer : _cloud_list)
      {
        if(buffer._buffer_color)
        {
          glDeleteBuffers(1, (GLuint *) &(buffer._buffer_color));
        }
        glDeleteBuffers(1, (GLuint *) &(buffer._buffer_pos));
        glDeleteVertexArrays(1, &buffer._id_array);
      }
      _mesh_list.clear();
      glfwTerminate();
    }

    void setDistance(const float distance)
    {
      _distance = distance;
      updateViewingMatrix();
    }

    /**
     * render in loop
     */
    void run()
    {
      while(glfwGetKey(_window, GLFW_KEY_ESCAPE) != GLFW_PRESS && glfwWindowShouldClose(_window) == 0)
      {
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        onRotationAndZooming();
        glm::mat4 proj = _mat_proj * _mat_view; // without its own tf

        // render mesh
        if(_id_program_colored_mesh)
        {
          renderColoredMesh(proj);
        }
        if(_id_program_unicolor_mesh)
        {
          renderUnicolorMesh(proj);
        }
        if(_id_program_colored_cloud_point)
        {
          renderColoredCloudPoint(proj);
        }
        if(_id_program_unicolor_cloud_point)
        {
          renderUnicolorCloudPoint(proj);
        }
        if(_id_program_unicolor_cloud_sphere)
        {
          renderUnicolorCloudSphere(proj);
        }
        if(_id_program_colored_cloud_sphere)
        {
          renderColoredCloudSphere(proj);
        }
        if(!_coordinate_sign_list.empty())
        {
          renderCoordinateSign(proj);
        }

        glfwSwapBuffers(_window);
        glfwPollEvents();
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

    void setTransform(const Transform &transform)
    {
      _mat_external = CommonVisualizer::transformToMat4(transform);
      updatePosViewer();
      updateUpViewer();
      updateCenterViewer();
      updateViewingMatrix();
    }

    /**
     * add mesh with uniform color
     * @param vertices list of vertex, every 9 floats is a trangle
     * @param color uniform color for this mesh
     */
    void addMesh(const std::vector<float> &vertices, const Vec &color)
    {
      assert(vertices.size() % 9 == 0);
      if(_id_program_unicolor_mesh == 0)
      {
        loadUnicolorMeshShader();
      }
      std::vector<float> normals = CommonVisualizer::genFakeNormal(vertices);
      _mesh_list.push_back(MeshBuffer());
//      _mesh_list.back()._is_colored = false;
      _mesh_list.back()._color = color;
      // vertex array
      glGenVertexArrays(1, &_mesh_list.back()._id_array);
      // vertex buffer
      CommonVisualizer::genBufferVectorFloat(vertices, _mesh_list.back()._buffer_vertex);
      // normal buffer
      CommonVisualizer::genBufferVectorFloat(normals, _mesh_list.back()._buffer_normal);
      // size
      _mesh_list.back()._size = (int) vertices.size() / 3;
      _mesh_list.back()._transform_local = glm::mat4(1.0f);
    }

    void addMesh(const std::vector<float> &vertices, const std::vector<int> &index, const Vec &color)
    {
      assert(vertices.size() % 9 == 0);
      assert(index.size() % 3 == 0);
      std::vector<float> triangle_vertices(index.size() * 3, 0.0f);
      for(size_t id = 0; id < index.size(); ++id)
      {
        const int i = index.at(id);
        assert(i * 3 < vertices.size());
        triangle_vertices.at(id * 3) = vertices.at(i * 3);
        triangle_vertices.at(id * 3 + 1) = vertices.at(i * 3 + 1);
        triangle_vertices.at(id * 3 + 2) = vertices.at(i * 3 + 2);
      }
      addMesh(triangle_vertices, color);
    }

    /**
     * add mesh with color
     * @param vertices list of vertex, every 9 floats is a trangle
     * @param colors list of color value, every 4 floats color on a vertex
     */
    void addMesh(const std::vector<float> &vertices, const std::vector<float> &colors)
    {
      assert(vertices.size() * 12 == colors.size() * 9);
      if(_id_program_colored_mesh == 0)
      {
        loadColoredMeshShader();
      }
      std::vector<float> normals = CommonVisualizer::genFakeNormal(vertices);
      _mesh_list.push_back(MeshBuffer());
//      _mesh_list.back()._is_colored = true;
      // vertex array
      glGenVertexArrays(1, &_mesh_list.back()._id_array);
      // vertex buffer
      CommonVisualizer::genBufferVectorFloat(vertices, _mesh_list.back()._buffer_vertex);
      // normal buffer
      CommonVisualizer::genBufferVectorFloat(normals, _mesh_list.back()._buffer_normal);
      // color buffer
      CommonVisualizer::genBufferVectorFloat(colors, _mesh_list.back()._buffer_color);
      // size
      _mesh_list.back()._size = (int) vertices.size() / 3;
      _mesh_list.back()._transform_local = glm::mat4(1.0f);
    }

    void addPointCloud(const std::vector<float> &points, const std::vector<float> &colors,
                       const float radius = -1.0f)
    {
      assert(points.size() * 4 == colors.size() * 3);
      _cloud_list.push_back(PointCloudBuffer());
      // vertex array
      glGenVertexArrays(1, &_cloud_list.back()._id_array);
      CommonVisualizer::genBufferVectorFloat(points, _cloud_list.back()._buffer_pos);
      CommonVisualizer::genBufferVectorFloat(colors, _cloud_list.back()._buffer_color);
      _cloud_list.back()._radius = radius;
      _cloud_list.back()._size = points.size() / 3;
      _cloud_list.back()._transform_local = glm::mat4(1.0f);
      if(radius <= 0.0f && _id_program_colored_cloud_point == 0)
      {
        loadColoredCloudPointShader();
      }
      else if(radius > 0.0f && _id_program_colored_cloud_sphere == 0)
      {
        loadColoredCloudSphereShader();
      }
    }

    void addPointCloud(const std::vector<float> &points, const Vec &color,
                       const float radius = -1.0f)
    {
      assert(points.size() % 3 == 0);
      _cloud_list.push_back(PointCloudBuffer());
      // vertex array
      glGenVertexArrays(1, &_cloud_list.back()._id_array);
      CommonVisualizer::genBufferVectorFloat(points, _cloud_list.back()._buffer_pos);
      _cloud_list.back()._color = color;
      _cloud_list.back()._radius = radius;
      _cloud_list.back()._size = points.size() / 3;
      _cloud_list.back()._transform_local = glm::mat4(1.0f);
      if(radius <= 0.0f && _id_program_unicolor_cloud_point == 0)
      {
        loadUnicolorCloudPointShader();
      }
      else if(radius > 0.0f && _id_program_unicolor_cloud_sphere == 0)
      {
        loadUnicolorCloudSphereShader();
      }
    }

    void addCoordinateSign(const Transform &transform)
    {
      glm::mat4 mat_tf = CommonVisualizer::transformToMat4(transform);
      std::vector<float> points;
      std::vector<float> colors;
      points.reserve(6 * 3);
      colors.reserve(6 * 4);

      // add points
      glm::vec4 start = mat_tf * glm::vec4(0.0f, 0.0f, 0.0f, 1.0f);
      for(int id_dim = 0; id_dim < 3; ++id_dim)
      {
        float color[] = {0.0f, 0.0f, 0.0f, 1.0f};
        glm::vec4 end(0.0f, 0.0f, 0.0f, 1.0f);
        end[id_dim] = 1.0f;
        end = mat_tf * end;
        color[id_dim] = 1.0f;
        points.push_back(start.x);
        points.push_back(start.y);
        points.push_back(start.z);
        points.push_back(end.x);
        points.push_back(end.y);
        points.push_back(end.z);
        for(int i = 0; i < 8; ++i)
        {
          colors.push_back(color[i % 4]);
        }
      }
      _coordinate_sign_list.push_back(PointCloudBuffer());
      glGenVertexArrays(1, &_coordinate_sign_list.back()._id_array);
      CommonVisualizer::genBufferVectorFloat(points, _coordinate_sign_list.back()._buffer_pos);
      CommonVisualizer::genBufferVectorFloat(colors, _coordinate_sign_list.back()._buffer_color);
      _coordinate_sign_list.back()._size = 6;
      _coordinate_sign_list.back()._transform_local = glm::mat4(1.0f);

      if(_id_program_colored_cloud_point == 0)
      {
        loadColoredCloudPointShader();
      }
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
      const char* texts[] = {vs, fs_phong, fs};
      GLenum types[] = {GL_VERTEX_SHADER, GL_FRAGMENT_SHADER, GL_FRAGMENT_SHADER};
      _id_program_colored_mesh = CommonVisualizer::createProgram(
          std::vector<GLenum>(types, types + 3), std::vector<const char*>(texts, texts + 3));
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
      const char* texts[] = {vs, fs_phong, fs};
      GLenum types[] = {GL_VERTEX_SHADER, GL_FRAGMENT_SHADER, GL_FRAGMENT_SHADER};
      _id_program_unicolor_mesh = CommonVisualizer::createProgram(
          std::vector<GLenum>(types, types + 3), std::vector<const char*>(texts, texts + 3));
    }

    void loadColoredCloudPointShader()
    {
      const char *vs =
#include "./shaders/vs_colored_cloud_point.vs"
      ;
      const char *fs =
#include "./shaders/fs_simple.fs"
      ;
      const char* texts[] = {vs, fs};
      GLenum types[] = {GL_VERTEX_SHADER, GL_FRAGMENT_SHADER};
      _id_program_colored_cloud_point = CommonVisualizer::createProgram(
          std::vector<GLenum>(types, types + 2), std::vector<const char*>(texts, texts + 2));
    }

    void loadUnicolorCloudPointShader()
    {
      const char *vs =
#include "./shaders/vs_unicolor_cloud_point.vs"
      ;
      const char *fs =
#include "./shaders/fs_simple.fs"
      ;
      const char* texts[] = {vs, fs};
      GLenum types[] = {GL_VERTEX_SHADER, GL_FRAGMENT_SHADER};
      _id_program_unicolor_cloud_point = CommonVisualizer::createProgram(
          std::vector<GLenum>(types, types + 2), std::vector<const char*>(texts, texts + 2));
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
      const char* texts[] = {gs, vs, fs, fs_phong};
      GLenum types[] = {GL_GEOMETRY_SHADER, GL_VERTEX_SHADER, GL_FRAGMENT_SHADER, GL_FRAGMENT_SHADER};
      _id_program_colored_cloud_sphere = CommonVisualizer::createProgram(
          std::vector<GLenum>(types, types + 4), std::vector<const char*>(texts, texts + 4));
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
      const char* texts[] = {gs, vs, fs, fs_phong};
      GLenum types[] = {GL_GEOMETRY_SHADER, GL_VERTEX_SHADER, GL_FRAGMENT_SHADER, GL_FRAGMENT_SHADER};
      _id_program_unicolor_cloud_sphere = CommonVisualizer::createProgram(
          std::vector<GLenum>(types, types + 4), std::vector<const char*>(texts, texts + 4));
    }
  };
}

#endif //COMMONVISUALIZER_COMMONVISUALIZER_HPP

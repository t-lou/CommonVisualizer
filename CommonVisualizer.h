//
// Created by tlou on 25.03.17.
//

#ifndef COMMONVISUALIZER_COMMONVISUALIZER_H_H
#define COMMONVISUALIZER_COMMONVISUALIZER_H_H

#include <list>
#include <string>
#include <vector>

#include "include/Container.hpp"
#include "include/Vec.h"
#include "include/include_gl.h"

namespace loco {
/// @brief constants for some useful colors
namespace color {
const Vec BLACK{0.0f, 0.0f, 0.0f, 1.0f};
const Vec GREY{0.5f, 0.5f, 0.5f, 1.0f};
const Vec WHITE{1.0f, 1.0f, 1.0f, 1.0f};
const Vec RED{1.0f, 0.0f, 0.0f, 1.0f};
const Vec GREEN{0.0f, 1.0f, 0.0f, 1.0f};
const Vec BLUE{0.0f, 0.0f, 1.0f, 1.0f};
const Vec YELLOW{1.0f, 1.0f, 0.0f, 1.0f};
const Vec MAGENTA{1.0f, 0.0f, 1.0f, 1.0f};
const Vec CYAN{0.0f, 1.0f, 1.0f, 1.0f};
}  // namespace color

/// @brief registeration of opengl programs for different types
union GlProgram {
  GLuint _ids[20];
  struct {
    GLuint _id_program_colored_mesh;
    GLuint _id_program_unicolor_mesh;
    GLuint _id_program_colored_cloud_point;
    GLuint _id_program_unicolor_cloud_point;
    GLuint _id_program_colored_cloud_sphere;
    GLuint _id_program_unicolor_cloud_sphere;
    GLuint _id_program_cylinder_side;
    GLuint _id_program_oriented_circle;
    GLuint _id_program_cone_side;
  };
};

class CommonVisualizer {
 private:
  /// @brief size of window
  const int _height, _width;
  /// @brief handler for the window
  GLFWwindow *_window;
  /// @brief whether initilization is successful
  bool _re_init;
  /// @brief 3D position of viewer
  float _distance, _theta, _phi;
  glm::vec4 _pos_viewer;
  glm::vec4 _up_viewer;
  glm::vec4 _center_viewer;
  LightSource _light_source;
  /// @brief parameter for phong shading
  const Vec _param_phong;
  /// @brief projection matrix
  const glm::mat4 _mat_proj;
  /// @brief viewing matrix, updated from interaction
  glm::mat4 _mat_view;
  glm::mat4 _transform_camera;
  /// @brief registration of gl programs (for different types)
  GlProgram _id_prog;
  /// @brief the objects to view
  Container _world;

  struct {
    /// @brief mouse position when button was pressed for interaction
    double _xpos_prev = 0.0;
    double _ypos_prev = 0.0;
    /// @brief whether the left button is pressed (rotation)
    bool _is_left_pressed = false;
    /// @brief whether the middle button is pressed (zooming)
    bool _is_middle_pressed = false;
    /// @brief whether the right button is pressed (translation)
    bool _is_right_pressed = false;
  } _interaction;

  /**
   * print error by compilation
   * @param length
   * @param id
   */
  static void dispErrorCompilation(const int length, const GLuint id,
                                   const GLenum &type);

  /**
   * load all text from a document. designed to load shader, but I want the
   * program to be independant to source at run time
   * @param filename
   * @return
   */
  static std::string loadSource(const std::string &filename);

  /**
   * create single shader with source code shader_text and shader type(vertex,
   * fragement...)
   * @param shader_type
   * @param shader_text
   * @return
   */
  static GLuint createShader(const GLenum &shader_type,
                             const char *shader_text);

  /**
   * create opengl program with vector of shaders and source codes
   * @param shader_types
   * @param shader_texts
   * @return
   */
  static GLuint createProgram(const std::vector<GLenum> &shader_types,
                              const std::vector<const char *> &shader_texts);

  /**
   * update position of viewer (eye) with external transform
   */
  void updatePosViewer();

  /**
   * update direction of up with external transform
   */
  void updateUpViewer();

  /**
   * update center (where to look at) with external transform
   */
  void updateCenterViewer();

  /**
   * update viewing matrix with position, look at position and up direction of
   * viewer (class member)
   */
  void updateViewingMatrix();

  /**
   * rotate and zoom(not implemented) according to mouse input
   */
  void onRotationAndZooming();

  /**
   * update viewer pose in program
   * @param id_program
   */
  void updateViewerPose(const GLuint &id_program);

  /**
   * prepare parameters for phong lighting to program
   * @param id_program
   */
  void updatePhongParameter(const GLuint &id_program);

  /**
   * update phong parameter in all created programs
   */
  void updatePhongParameter();

 public:
  CommonVisualizer(const int height = 600, const int width = 800,
                   const char *name = "default",
                   const Vec &background = color::BLACK);

  virtual ~CommonVisualizer();

  /**
   * clear all objects
   */
  void resetScene();

  /**
   * set distance from eye to center (zooming)
   * @param distance
   */
  void setDistance(const float distance);

  /**
   * set theta of viewer, which is the angle of viewing direction to -Z
   * @param theta
   */
  void setTheta(const float theta);

  /**
   * set phi of view, which is rotation around Z-axis
   * @param phi
   */
  void setPhi(const float phi);

  /**
   * displace once
   * @return whether to continue
   */
  bool playOnce();

  /**
   * render in loop
   */
  void play();

  /**
   * checks intialiazation
   * @return whether the initialization is correct
   */
  bool isInited() const;

  /**
   * add point light source (other are not necessary, point source shows the
   * shape well)
   * @param pos
   * @param color
   */
  void setLightSource(const Vec &pos, const Vec &color);

  /**
   * move camera with translation and rotation
   * @param transform
   */
  void setTransformCamera(const Transform &transform);

  void setTransform(const Transform &transform);

  void setTransform(const int id, const Transform &transform);

  /**
   * add mesh with uniform color
   * @param vertices list of vertex, every 9 floats is a trangle
   * @param color uniform color for this mesh
   */
  void addMesh(const std::vector<float> &vertices, const Vec &color);

  /**
   * add mesh with vertices and indices for triangles
   * @param vertices
   * @param index
   * @param color
   */
  void addMesh(const std::vector<float> &vertices,
               const std::vector<int> &index, const Vec &color);

  /**
   * add mesh with color
   * @param vertices list of vertex, every 9 floats is a trangle
   * @param colors list of color value, every 4 floats color on a vertex
   */
  void addMesh(const std::vector<float> &vertices,
               const std::vector<float> &colors);

  /**
   *
   * @param points
   * @param colors
   * @param radius
   */
  void addPointCloud(const std::vector<float> &points,
                     const std::vector<float> &colors,
                     const float radius = -1.0f);

  void addPointCloud(const std::vector<float> &points, const Vec &color,
                     const float radius = -1.0f);

  /**
   * add the notation for coordinate system after transform
   * @param transform
   */
  void addCoordinateSign(const Transform &transform);

  /**
   * add cylinder to visualizer
   * @param positions vector of positions of length 6*num, for each arrow:
   *        from_x, from_y, from_z, to_x, to_y, to_z
   * @param radius
   * @param colors
   */
  void addCylinder(const std::vector<float> &positions,
                   const std::vector<float> &radius,
                   const std::vector<Vec> &colors);

  /**
   * add capsule to visualizer
   * @param positions vector of positions of length 6*num, for each arrow:
   *        from_x, from_y, from_z, to_x, to_y, to_z
   * @param radius
   * @param colors
   */
  void addCapsule(const std::vector<float> &positions,
                  const std::vector<float> &radius,
                  const std::vector<Vec> &colors);

  /**
   * add cone to visualizer
   * @param positions vector of positions of length 6*num, for each arrow:
   *        from_x, from_y, from_z, to_x, to_y, to_z
   * @param radius
   * @param colors
   */
  void addCone(const std::vector<float> &positions,
               const std::vector<float> &radius,
               const std::vector<Vec> &colors);

  /**
   * add arrows to cisualizer
   * @param positions vector of positions of length 6*num, for each arrow:
   *        from_x, from_y, from_z, to_x, to_y, to_z
   * @param length_head
   * @param radius_head
   * @param radius_body
   * @param colors
   */
  void addArrow(const std::vector<float> &positions,
                const std::vector<float> &length_head,
                const std::vector<float> &radius_head,
                const std::vector<float> &radius_body,
                const std::vector<Vec> &colors);

  /**
   * add box with transform, scaling and one color
   * @param transform
   * @param scale
   * @param color
   */
  void addBox(const Transform &transform, const Vec &scale, const Vec &color);

  /**
   * load shader for mesh rendering from source
   */
  void loadColoredMeshShader();

  /**
   * load shader for mesh with color rendering from source
   */
  void loadUnicolorMeshShader();

  /**
   * load shader for colored point cloud rendering from source
   */
  void loadColoredCloudPointShader();

  /**
   * load shader for point cloud with unified color from source
   */
  void loadUnicolorCloudPointShader();

  /**
   * load shader for spheres, which has its own color, from source
   */
  void loadColoredCloudSphereShader();

  /**
   * load shader for spheres with same color
   */
  void loadUnicolorCloudSphereShader();

  /**
   * load shader for side of cylinder
   */
  void loadCylinderSideShader();

  /**
   * load shader for circle rendering
   */
  void loadOrientedCircleShader();

  /**
   * load shader for side of cones
   */
  void loadConeSideShader();
};
}  // namespace loco

#endif  // COMMONVISUALIZER_COMMONVISUALIZER_H_H
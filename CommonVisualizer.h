//
// Created by tlou on 25.03.17.
//

#ifndef COMMONVISUALIZER_COMMONVISUALIZER_H_H
#define COMMONVISUALIZER_COMMONVISUALIZER_H_H

#include <cstdint>
#include <list>
#include <map>
#include <string>
#include <unordered_map>
#include <vector>

#include "include/Container.hpp"
#include "include/DataStructure.h"
#include "include/Object.hpp"

namespace loco {
/// @brief constants for some useful colors
namespace color {
const Vec BLACK{{0.0f, 0.0f, 0.0f, 1.0f}};
const Vec GREY{{0.5f, 0.5f, 0.5f, 1.0f}};
const Vec WHITE{{1.0f, 1.0f, 1.0f, 1.0f}};
const Vec RED{{1.0f, 0.0f, 0.0f, 1.0f}};
const Vec GREEN{{0.0f, 1.0f, 0.0f, 1.0f}};
const Vec BLUE{{0.0f, 0.0f, 1.0f, 1.0f}};
const Vec YELLOW{{1.0f, 1.0f, 0.0f, 1.0f}};
const Vec MAGENTA{{1.0f, 0.0f, 1.0f, 1.0f}};
const Vec CYAN{{0.0f, 1.0f, 1.0f, 1.0f}};
}  // namespace color

/// @brief registeration of opengl programs for different types
union GlProgram {
  GLuint _ids[20];
  struct Named {
    GLuint _id_program_colored_mesh;
    GLuint _id_program_unicolor_mesh;
    GLuint _id_program_colored_cloud_point;
    GLuint _id_program_unicolor_cloud_point;
    GLuint _id_program_colored_cloud_sphere;
    GLuint _id_program_unicolor_cloud_sphere;
    GLuint _id_program_cylinder_side;
    GLuint _id_program_oriented_circle;
    GLuint _id_program_cone_side;
  } _named;
};

class CommonVisualizer {
 private:
  /// @brief size of window
  const std::uint32_t _height, _width;
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
  std::string _name_world{""};
  std::map<std::string, Container> _world;

  struct {
    /// @brief mouse position when button was pressed for interaction
    double xpos_prev = 0.0;
    double ypos_prev = 0.0;
    /// @brief whether one key or button is pressed (rotation)
    std::unordered_map<std::uint32_t, bool> is_key_pressed{
        {GLFW_KEY_W, false},
        {GLFW_KEY_S, false},
        {GLFW_KEY_A, false},
        {GLFW_KEY_D, false},
        {GLFW_KEY_N, false},
        {GLFW_KEY_F, false},
        {GLFW_KEY_RIGHT, false},
        {GLFW_KEY_LEFT, false},
        {GLFW_KEY_DOWN, false},
        {GLFW_KEY_UP, false},
        {GLFW_MOUSE_BUTTON_LEFT, false},
        {GLFW_MOUSE_BUTTON_RIGHT, false},
        {GLFW_MOUSE_BUTTON_MIDDLE, false}};
  } _interaction;

  /**
   * print error by compilation
   * @param length
   * @param id
   */
  static void dispErrorCompilation(const GLint length, const GLuint id,
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

  /**
   * rotate the polar coordinates for viewing
   */
  void rotateView(const float dx, const float dy);

  /**
   * move the viewing position in up and right direction
   */
  void translateView(const float dx, const float dy);

  /**
   * shrink or enlarge the viewing distance with factor
   */
  void changeDistance(const float dy);

 public:
  CommonVisualizer(const std::uint32_t height = 600u,
                   const std::uint32_t width = 800u,
                   const char *name = "default",
                   const Vec &background = color::BLACK);

  virtual ~CommonVisualizer();

  /**
   * set the name of world to change, default is ""
   */
  void setActiveWorld(const std::string &name);

  /**
   * clear all objects
   */
  void resetScene();

  /**
   * remove part of a scene with given name
   */
  void resetScene(const std::string &name);

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

  void setTransform(const std::uint32_t id, const Transform &transform);

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
               const std::vector<std::uint32_t> &index, const Vec &color);

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
  void addCoordinateSign(const Transform &transform, const float scale = 1.0f,
                         const bool is_line = false);

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
   * add line consists of capsules to visualizer
   * @param positions vector of positions of length 3*num: x0,y0,z0,x1,y0,z0...
   * @param radius
   * @param colors
   */
  void addLine(const std::vector<float> &positions, const float radius,
               const Vec &color);

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
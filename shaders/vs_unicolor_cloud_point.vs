R"(#version 330 core

layout(location = 0) in vec3 pos;

out float depth_fs_in;
out vec4 color_fs_in;

uniform mat4 transform;
uniform mat4 proj;
uniform vec4 color;
uniform float point_size;

void main()
{
  gl_Position = proj * transform * vec4(pos, 1.0f);
  gl_Position /= gl_Position.w;
  gl_PointSize = point_size;
  color_fs_in = color;
  depth_fs_in = gl_Position.z;
}
)"